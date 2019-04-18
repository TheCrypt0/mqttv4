#ifndef MQTTV4_H
#define MQTTV4_H

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "ipc.h"
#include "mqtt.h"

#define MQTTV4_VERSION      "0.0.1"
#define MQTTV4_CONF_FILE    "/home/yi-hack-v4/etc/mqttv4.conf"

typedef struct
{
    char *mqtt_prefix;
    char *topic_motion;
    char *motion_start_msg;
    char *motion_stop_msg;
} mqttv4_conf_t;

#endif // MQTTV4_H
