// A simple C program to introduce
// a linked list
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

/*
*   Adds node to linked list
*/
void addNode(struct Node** head_ref, char *new_topic)
{
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
    struct Node *last = *head_ref;

    new_node->topic = (char*) malloc(sizeof(new_topic) * sizeof(char*));
    strcpy(new_node->topic, new_topic);
    new_node->topic_event = NULL;
    new_node->next = NULL;

    if(*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }

    while(last->next != NULL){
        last = last->next;
    }

    last->next = new_node;
    return;
}

/*
*   Add event to matching linked list node
*   Events are matched by matching topics
*/
void addEventToNode(struct Node **head_ref, Event *new_event)
{
    struct Node *last = *head_ref;
    Event *ev = (Event *) malloc(sizeof *ev);
    Event *last_event = NULL;

    ev->topic=(char*) malloc(sizeof(new_event->topic) * (sizeof(char*));
    ev->parameter_name=(char*) malloc(sizeof(new_event->parameter_name) * sizeof(char*));
    ev->parameter_type=(char*) malloc(sizeof(new_event->parameter_type) * sizeof(char*));
    ev->expectedValue=(char*) malloc(sizeof(new_event->expectedValue) * sizeof(char*));

    strcpy(ev->topic, new_event->topic);
    strcpy(ev->parameter_name, new_event->parameter_name);
    strcpy(ev->parameter_type, new_event->parameter_type);
    strcpy(ev->expectedValue, expectedValue);
    ev->condition = new_event->condition;
    ev->next_event = NULL;
  
    while(last != NULL){
        if(strcmp(last->topic, ev->topic) == 0){
            if(last->topic_event == NULL){
                last->topic_event = ev;
                return;
            }
            last_event = last->topic_event;
            while(last_event->next_event != NULL){
                last_event = last_event->next_event;
            }
            last_event->next_event = ev;
            return;
        }
        last = last->next;
    }
    return;
}

/*
*   Free's input event and all following events memory
*   Must supply a non empty event 
*   Gets called by deleteList() function below
*/

static void deleteEvents(Event *ev)
{
    Event *tempEv;
    while (ev != NULL){
        tempEv = ev;
        ev = ev->next_event;
        free(tempEv->topic);
        free(tempEv->parameter_name);
        free(tempEv->parameter_type);
        free(tempEv->expectedValue);
        free(tempEv);
    }
}

/*
*   Deletes all linked lists, starting from last one
*   If event exists calls deleteEvents() to delete matching events
*   Must check if event exists for deleteEvents()
*/

void deleteList(struct Node *n)
{
    struct Node *temp;

    while (n != NULL){
        temp = n;
        n = n->next;
        deleteEvents(temp->topic_event);
        free(temp->topic);
        free(temp);
    }

}
