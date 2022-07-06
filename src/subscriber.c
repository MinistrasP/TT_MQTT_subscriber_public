#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <ctype.h>
#include "cJSON.h"
#include "linkedList.h"
#include "subscriber.h"
#include "database.h"
#include "emailClient.h"


extern struct Node* head;
extern userData* uDat;

/*
*   Send email with supplied data
*   Gets called after check_value_string()
*/
static int prepare_email(Event *tempEvent, char *input_string, char *input_conditionString)
{
    if(send_email(uDat->user_email, uDat->destination_email, tempEvent->topic, tempEvent->parameter_name, input_string, tempEvent->expectedValue, input_conditionString) != 0){
        syslog(LOG_WARNING, "MQTT subsriber: Failed to send email for topic <%s>", tempEvent->topic);
        return 1;
    }
    return 0;
} 
/*
*   Checks if input value matches expected value based on conditions
*   Condition is specified in Event typedef which is passed for this topic
*   Calls a function to send email if event condition matches
*/
static int check_value_string(Event *tempEvent, char *input_string)
{
    if(tempEvent->condition == 3){
        if(strcmp(tempEvent->expectedValue, input_string) == 0){
            if(prepare_email(tempEvent, input_string, "Input string is equal to expected value") != 0){
                return 1;
            }
        }
    }
    else if (tempEvent->condition == 4){
        if(strcmp(tempEvent->expectedValue, input_string) != 0){
            if(prepare_email(tempEvent, input_string, "Input string is not equal to expected value") != 0){
                return 1;
            }
        }
    }
    else{
        syslog(LOG_WARNING, "MQTT subsriber: Wrong condition for type <%s>",tempEvent->parameter_type);
        return 1;
    }
    return 0;
}
static int check_value_int(Event *tempEvent, int input_int)
{
    char *tempCharInt = (char *) malloc(sizeof(input_int) * sizeof(char));
    int input_expected_int = atoi(tempEvent->expectedValue);
    sprintf(tempCharInt, "%d", input_int); 

    if(tempEvent->condition == 1){
        if(input_expected_int < input_int){
            prepare_email(tempEvent, tempCharInt, "Input number is higher than expected value");
        }
    }
    else if ((tempEvent->condition == 2)){
        if(input_expected_int > input_int){
            prepare_email(tempEvent, tempCharInt, "Input number is lower than expected value");
        }
    }
    else if ((tempEvent->condition == 3)){
        if(input_expected_int == input_int){
            prepare_email(tempEvent, tempCharInt, "Input number is equal to expected value");
        }
    }   
    else if ((tempEvent->condition == 4)){
        if(input_expected_int != input_int){
            prepare_email(tempEvent, tempCharInt, "Input number is not equal to expected value");
        }
    }
    else if ((tempEvent->condition == 5)){
        if(input_expected_int >= input_int){
            prepare_email(tempEvent, tempCharInt, "Input number is lower or equal to expected value");
        }
    }
    else if ((tempEvent->condition == 6)){
        if(input_expected_int <= input_int){
            prepare_email(tempEvent, tempCharInt, "Input number is higher or equal to expected value");
        }
    }
    else{
    }
    free(tempCharInt);
    return 0;
}

static int checkSenderInfo()
{
    if((uDat->destination_email == NULL) || (uDat->email_password == NULL) || (uDat->user_email == NULL)){
        return 1;
    }
    else{
        return 0;
    }
}

static int checkJsonFormatting(cJSON **inputJson, char *input_payload)
{
    *inputJson = cJSON_Parse(input_payload);
    if (*inputJson == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            return 1;
        }    
    }
    else{
        return 0;
    }
    return 1;
}

static int checkEventExists(struct Node *n, Event **tempEvent, char *input_topic)
{
    while(n != NULL){
        if(strcmp(n->topic, input_topic)== 0){
            if(n->topic_event != NULL){
                *tempEvent = n->topic_event;
                return 0;
            }
        }
    n = n->next;
    }
    return 1;
}

static int checkAllEvents(Event *tempEvent, cJSON *inputJSON)
{
    cJSON *name = NULL;
    while(tempEvent != NULL){
        name = cJSON_GetObjectItemCaseSensitive(inputJSON, tempEvent->parameter_name);
        if(strcmp(tempEvent->parameter_type, "string") == 0){
            if (cJSON_IsString(name) && (name->valuestring != NULL))
            {
                check_value_string(tempEvent, name->valuestring);
            }
        }
        else if(strcmp(tempEvent->parameter_type, "int") == 0){
            if (cJSON_IsNumber(name) && (name->valueint != NULL))
            {
                check_value_int(tempEvent, name->valueint);
            }
        }
        else{

        }
        tempEvent = tempEvent->next_event;
    }
    return 1;
}

void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	int rc;
    syslog(LOG_INFO, "MQTT subsriber: MQTT connection %s", mosquitto_connack_string(reason_code));

	if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}
	struct Node* temp_node = (struct Node*) malloc(sizeof(struct Node));
	temp_node->topic = (char*)malloc(sizeof(char*));
	temp_node->topic = head->topic;
	temp_node->next = head->next;

	while(temp_node != NULL){
        	if(temp_node->topic != NULL){
           		rc = mosquitto_subscribe(mosq, NULL, temp_node->topic, 1);
			if(rc != MOSQ_ERR_SUCCESS){
				syslog(LOG_WARNING, "MQTT subsriber: Error subscribing to topic %s, error message %s", temp_node->topic, mosquitto_strerror(rc));
			}
		}
		temp_node = temp_node->next;
    }
	free(temp_node);
}	

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	int i;
	bool have_subscription = false;

	for(i=0; i<qos_count; i++){
        syslog(LOG_INFO, "MQTT subsriber: Subscribed to topic [%d]", mid);
		if(granted_qos[i] <= 2){
			have_subscription = true;
		}
	}
	if(have_subscription == false){
        syslog(LOG_ERR, "MQTT subsriber: All subscriptions rejected. Exiting program");
		mosquitto_disconnect(mosq);
	}
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    const cJSON *name = NULL;
    cJSON *myString_json = NULL;
    Event *currentEvent = NULL;

    insertData(msg->topic, (char *)msg->payload);
    if(checkSenderInfo() != 0){
        return;
    }
    if(checkJsonFormatting(&myString_json, msg->payload) == 1){
        return;
    }
    if(checkEventExists(head, &currentEvent, msg->topic) == 1){
        return;
    }
    checkAllEvents(currentEvent, myString_json);
    cJSON_Delete(myString_json);
}
