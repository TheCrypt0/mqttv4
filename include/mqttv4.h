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
#include "files.h"

#define MQTTV4_VERSION      "0.0.3"
#define MQTTV4_CONF_FILE    "/home/yi-hack-v4/etc/mqttv4.conf"

typedef struct
{
    char *mqtt_prefix;
    char *topic_birth;
    char *topic_will;
    char *topic_motion;
    char *topic_motion_files;
    char *topic_baby_crying;
    char *birth_msg;
    char *will_msg;
    char *motion_start_msg;
    char *motion_stop_msg;
    char *baby_crying_msg;
} mqttv4_conf_t;

#endif // MQTTV4_H
