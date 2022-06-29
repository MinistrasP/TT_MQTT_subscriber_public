#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include "linkedList.h"
#include "emailClient.h"

#define DEFAULTPAYLOADSIZE 200
static char *payload_text;
extern userData* uDat;

static struct upload_status {
  size_t bytes_read;
};
 
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
 
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
 
  data = &payload_text[upload_ctx->bytes_read];
 
  if(data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
 
    return len;
  }
 
  return 0;
}
 
int send_email(char *from, char *to, char *topic, char *parameterName, char *data, char *expectedData, char *condition)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };
  payload_text = (char*) malloc((strlen(from)+strlen(to)+strlen(topic)+strlen(parameterName)+strlen(data)+strlen(expectedData)+strlen(condition)+DEFAULTPAYLOADSIZE)*sizeof(char));
  char *from_mail;
  char *to_mail;
  from_mail = (char*) malloc((strlen(from)+3)*sizeof(char));
  to_mail = (char*) malloc((strlen(to)+3)*sizeof(char));
  sprintf(from_mail,"<%s>",from);
  sprintf(to_mail,"<%s>",to);

  curl = curl_easy_init();
  sprintf(payload_text, "From: Kasparas Elzbutas  <%s> \r\n"
                          "To: Kasparas Elzbutas  <%s> \r\n"
                          "Subject: MQTT Rutx11\r\n"
                          "\r\n"
                          "Topic: %s\r\n"
                          "Parameter Name: %s\r\n"
                          "Data: %s\r\n"
                          "Expected Data: %s\r\n"
                          "Condition: %s\r\n"
                          ,from,to,topic,parameterName,data,expectedData,condition);
  

    if(curl) {

      curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
      curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_mail);
  
      recipients = curl_slist_append(recipients, to_mail);
      curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
  
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
      curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

      curl_easy_setopt(curl, CURLOPT_USERNAME, from);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, uDat->email_password);
      curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
  
      res = curl_easy_perform(curl);
  
      if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
      }
      curl_slist_free_all(recipients);
      curl_easy_cleanup(curl);
    }
  free(to_mail);
  free(from_mail);
  free(payload_text);
  return (int)res;
}