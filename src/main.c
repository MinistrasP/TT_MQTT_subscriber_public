#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <uci.h>
#include <syslog.h>
#include <signal.h>
#include "database.h"
#include "uci_reader.h"
#include "subscriber.h"
#include "linkedList.h"
#include "emailClient.h"

userData *uDat = NULL;
struct Node *head = NULL;

static void cleanup()
{
    mosquitto_lib_cleanup();
    free(uDat->destination_email);
    free(uDat->email_password);
    free(uDat->ipAddress);
    free(uDat->password);
    free(uDat->user_email);
    free(uDat->username);
    free(uDat);
    deleteList(head);  
    //Sutvarkyt UCIfreecontext
    closeDatabase();
    closelog();
}

void sigHandler(int s){
    syslog(LOG_ERR, "MQTT subsriber: Received signal: %d, terminating program\n", s);
    cleanup();
    exit(1); 

}

static int setupMQTTSubscriber(void)
{
    struct mosquitto *mosq;
    int rc = 0;

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

    if(uDat->tls == 1){
        mosquitto_tls_set(mosq, "/etc/certificates/ca.cert.pem", NULL, NULL, NULL, NULL);
    }

    mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);

    mosquitto_username_pw_set(mosq, uDat->username, uDat->password);
	rc = mosquitto_connect(mosq, uDat->ipAddress, uDat->port, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
        syslog(LOG_ERR, "MQTT subsriber: Error - %s\n ", mosquitto_strerror(rc));
		return 1;
	}
    rc = createDb();
    if(rc != 0){
        syslog(LOG_ERR, "MQTT subsriber: Failed to create Database");
        mosquitto_destroy(mosq);
        closelog();
        return 1;
    }
    rc = openDatabase();
    if(rc != 0){
        syslog(LOG_ERR, "MQTT subsriber: Failed to open database");
        mosquitto_destroy(mosq);
        closelog();
        return 1;
    }
	mosquitto_loop_forever(mosq, -1, 1);

    return 0;
}

int main (void)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sigHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    int rc = 0;

    openlog("syslog_log", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "MQTT subsriber: Started");

    rc = uci_read();
    if(rc != 0){
        syslog(LOG_ERR, "MQTT subsriber: Failed to read UCI data");
        closelog();
        return 1;
    }
    
    rc = setupMQTTSubscriber();
    if(rc != 0){
        syslog(LOG_INFO, "MQTT subsriber: Failed to setup MQTT data");
        cleanup();
        return 1;
    }
    cleanup();

    return 0;
}