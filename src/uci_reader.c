#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uci.h>
#include <mosquitto.h>
#include "linkedList.h"
#include "subscriber.h"
#include "database.h"
#include "uci_reader.h"

extern userData* uDat;
extern struct Node* head;

static char *uci_list_to_string(struct uci_list *list)
{
    size_t max_size = 100;
    size_t current_size = 1;
    char *result = calloc(max_size, sizeof(char));
    struct uci_element *i;

    uci_foreach_element(list, i)
    {
        size_t element_length = strlen(i->name) + 2;
        char name[element_length + 2];
        strcpy(name, i->name);

        if (current_size + element_length > max_size)
        {
            max_size = max_size * 2 + element_length;
            result = realloc(result, sizeof(char) * max_size);
        }

        strcat(result, strcat(name, ", "));
        current_size += element_length;
    }
    if (current_size >= 3)
        result[current_size - 3] = '\0';
    else
        result[0] = '\0';

    return result;
}

static int userDataInput(userData* userData, char *option_name, char *option)
{
    if(strstr(option_name, "username"))
    {
        uDat->username = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(uDat->username, option);
    }
    if(strstr(option_name, "password"))
    {
        uDat->password = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(uDat->password, option);
    }
    if(strstr(option_name, "ip_address"))
    {
        uDat->ipAddress = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(uDat->ipAddress, option);
    }
    if(strstr(option_name, "port"))
    {
        uDat->port = atoi(option);
    }
    if(strstr(option_name, "tls"))
    {
        uDat->tls = atoi(option);
    }
    if(strstr(option_name, "user_email"))
    {
        uDat->user_email = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(uDat->user_email, option);
    }
    if(strstr(option_name, "email_pass"))
    {
        uDat->email_password = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(uDat->email_password, option);
    }
    if(strstr(option_name, "destination_email"))
    {
        uDat->destination_email = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(uDat->destination_email, option);
    }
    return 0;
}

static int topicDataInput(char *option_name, char *option)
{
    if(strstr(option_name, "topic"))
    {
        addNode(&head, option);
    }
    return 0;
}

static int eventDataInput(Event* event, char *option_name, char *option)
{
    if(strstr(option_name, "topic"))
    {
        event->topic = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(event->topic, option);
    }
    if(strstr(option_name, "parameter_name"))
    {
        event->parameter_name = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(event->parameter_name, option);
    }
    if(strstr(option_name, "parameter_type"))
    {
        event->parameter_type = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(event->parameter_type, option);
    }
    if(strstr(option_name, "condition"))
    {
        event->condition = atoi(option);
    }
    if(strstr(option_name, "expectedValue"))
    {
        event->expectedValue = (char*) malloc(sizeof(option) * sizeof(char*));
        strcpy(event->expectedValue, option);
        addEventToNode(&head, event);
    }
    return 0;
}

int uci_read(void)
{
    const char *config_name = "mqtt_sub"; //.config filename
    struct uci_context *context = uci_alloc_context();
    struct uci_package *package;

    uDat = malloc(sizeof *uDat);

    if (uci_load(context, config_name, &package) != UCI_OK)
    {
        uci_perror(context, "uci_load()");
        uci_free_context(context);
        return 1;
    }

    struct uci_element *i, *j;
    uci_foreach_element(&package->sections, i)
    {
        struct uci_section *section = uci_to_section(i);
        char *section_type = section->type;
        char *section_name = section->e.name;

        if(strstr(section_type, "configuration")){
            uci_foreach_element(&section->options, j){
                struct uci_option *option = uci_to_option(j);
                char *option_name = option->e.name;
                
                if (option->type == UCI_TYPE_STRING){
                    userDataInput(uDat, option_name, option->v.string);
                }
                else
                {
                    char *list = uci_list_to_string(&option->v.list);
                    userDataInput(uDat, option_name, list);
                }
            }
        }
        if(strstr(section_type, "topic")){
            uci_foreach_element(&section->options, j){
                struct uci_option *option = uci_to_option(j);
                char *option_name = option->e.name;
                
                if (option->type == UCI_TYPE_STRING){
                    topicDataInput(option_name, option->v.string);
                }
                else
                {
                    char *list = uci_list_to_string(&option->v.list);
                    topicDataInput(option_name, list);
                    free(list);
                }
            }
        }   
        if(strstr(section_type, "event")){
            Event *ev = (Event *) malloc(sizeof *ev);

            uci_foreach_element(&section->options, j){
                struct uci_option *option = uci_to_option(j);
                char *option_name = option->e.name;
                
                if (option->type == UCI_TYPE_STRING){
                    eventDataInput(ev, option_name,option->v.string);
                }
                else
                {
                    char *list = uci_list_to_string(&option->v.list);
                    eventDataInput(ev, option_name, list);
                    free(list);
                }
            }
        }
    }
    return 0;
}
