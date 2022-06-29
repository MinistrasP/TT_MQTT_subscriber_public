// A simple C program to introduce
// a linked list
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

/*
*   Add node to linked list
*/
void addNode(struct Node** head_ref, char *new_topic)
{
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
    struct Node *last = *head_ref;

    new_node->topic = (char*)malloc(sizeof(char*));
    new_node->topic = new_topic;
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

    ev->topic=(char*)malloc(sizeof(char*));
    ev->parameter_name=(char*)malloc(sizeof(char*));
    ev->parameter_type=(char*)malloc(sizeof(char*));
    ev->expectedValue=(char*)malloc(sizeof(char*));

    ev->topic = new_event->topic;
    ev->parameter_name = new_event->parameter_name;
    ev->parameter_type = new_event->parameter_type;
    ev->expectedValue = new_event->expectedValue;
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
*   Prints whole linked list
*   Each node prints its matched event (work in progress)
*/
void printList(struct Node *n)
{
    Event *tempEvent = NULL;
    struct Node *tempNode = n;
    while(tempNode != NULL){
        if(tempNode->topic_event != NULL){
            tempEvent = tempNode->topic_event;
            while(tempEvent != NULL){
                tempEvent = tempEvent->next_event;
            }
        }
        tempNode = tempNode->next;
    }
    free(tempEvent);
}

void deleteEvents(Event **ev)
{
    Event *tempEvent;
    if((*ev)->next_event == NULL){
        free((*ev)->expectedValue);
        free((*ev)->parameter_name);
        free((*ev)->parameter_type);
        free((*ev)->topic);
        free(ev);
        return;
    }
    else{
        while((*ev)->next_event != NULL){
            tempEvent = (*ev)->next_event;
            while(tempEvent->next_event != NULL){
                tempEvent = tempEvent->next_event;
            }
            free(tempEvent->expectedValue);
            free(tempEvent->parameter_name);
            free(tempEvent->parameter_type);
            free(tempEvent->topic);
            free(tempEvent);
        }
        return;
    }
}

/*
*   Deletes all linked list and matching events by interations
*/

void deleteList(struct Node **n)
{
    struct Node *lastTempNode = NULL;
    if((*n) == NULL){
        return;
    }
    else if((*n)->next == NULL){
        if((*n)->topic_event != NULL ){
            deleteEvents(&((*n)->topic_event));
        }
        goto end;
    }
    else{
        while((*n)->next != NULL){
            lastTempNode = (*n)->next;
            while(lastTempNode->next != NULL){
                lastTempNode = lastTempNode->next;
            }
            deleteEvents(&((*n)->topic_event));
            free(lastTempNode->topic);
            free(lastTempNode);
        }
        return;
    }


    end:
        free((*n)->topic);
        free(n);
        return;
}
