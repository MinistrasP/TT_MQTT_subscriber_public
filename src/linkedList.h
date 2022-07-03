typedef struct
{
    char *username;
    char *password;
    int tls;
    int port;
    char *ipAddress;
    char *user_email;
    char *email_password;
    char *destination_email;
}userData;

typedef struct Event
{
    char *topic;
    char *parameter_name;
    char *parameter_type;
    int   condition;
    char *expectedValue;
    struct Event *next_event;
} Event;

struct Node {
    char *topic;
    Event* topic_event;
	struct Node* next;
};

void addNode(struct Node **head_ref, char *new_topic);
void addEventToNode(struct Node **head_ref, Event *new_event);
void deleteList(struct Node *n);