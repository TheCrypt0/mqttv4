#ifndef MQTT_H
#define MQTT_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <mosquitto.h>

#define MAX_RETRY   20

typedef struct
{
    char        host[32];
    int         port;
    int         keepalive;
    char        bind_address[32];
    int         qos;
    int         retain;
} mqtt_conf_t;

typedef struct
{
    char* topic;
    char* msg;
    int len;
} mqtt_msg_t;

int init_mqtt(void);
void stop_mqtt(void);

void mqtt_loop(void);

void mqtt_init_conf(mqtt_conf_t *conf);
void mqtt_set_conf(mqtt_conf_t *conf);
int mqtt_send_message(mqtt_msg_t *msg);

int mqtt_connect();

#endif // MQTT_H
