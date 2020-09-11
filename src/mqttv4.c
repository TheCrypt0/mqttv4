#include "mqttv4.h"

/*
 * Just a quick disclaimer.
 * This code is ugly and it is the result of some testing
 * and madness with the MQTT.
 *
 * It will be probably re-written from the ground-up to
 * handle more configurations, messages and callbacks.
 *
 * Warning: No clean exit.
 *
 * Crypto
 */

mqtt_conf_t conf;
mqttv4_conf_t mqttv4_conf;

static void init_mqttv4_config();
static void handle_config(const char *key, const char *value);

void callback_motion_start()
{
    char topic[128];
    mqtt_msg_t msg;

    printf("CALLBACK MOTION START\n");

    msg.msg=mqttv4_conf.motion_start_msg;
    msg.len=strlen(msg.msg);
    msg.topic=topic;

    sprintf(topic, "%s/%s", mqttv4_conf.mqtt_prefix, mqttv4_conf.topic_motion);

    mqtt_send_message(&msg);
}

void callback_motion_stop()
{
    char topic[128];
    mqtt_msg_t msg;

    printf("CALLBACK MOTION STOP\n");

    msg.msg=mqttv4_conf.motion_stop_msg;
    msg.len=strlen(msg.msg);
    msg.topic=topic;

    sprintf(topic, "%s/%s", mqttv4_conf.mqtt_prefix, mqttv4_conf.topic_motion);

    mqtt_send_message(&msg);
}

int main(int argc, char **argv)
{
    int ret;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    printf("Starting mqttv4 v%s\n", MQTTV4_VERSION);

    mqtt_init_conf(&conf);
    init_mqttv4_config();
    mqtt_set_conf(&conf);

    ret=init_mqtt();
    if(ret!=0)
        exit(EXIT_FAILURE);

    ret=mqtt_connect();
    if(ret!=0)
        exit(EXIT_FAILURE);

    ret=ipc_init();
    if(ret!=0)
        exit(EXIT_FAILURE);

    ipc_set_callback(IPC_MSG_MOTION_START, &callback_motion_start);
    ipc_set_callback(IPC_MSG_MOTION_STOP, &callback_motion_stop);

    while(1)
    {
        mqtt_check_connection();
        mqtt_loop();
        usleep(500*1000);
    }

    return 0;
}

static void handle_config(const char *key, const char *value)
{
    int nvalue;

    // Ok, the configuration handling is UGLY, unsafe and repetitive.
    // It should be fixed in the future by writing a better config
    // handler or by just using a library.

    // If you think to have a better implementation.. PRs are welcome!

    if(strcmp(key, "MQTT_IP")==0)
    {
        strcpy(conf.host, value);
    }
    else if(strcmp(key, "MQTT_CLIENT_ID")==0)
    {
        strcpy(conf.client_id, value);
    }
    else if(strcmp(key, "MQTT_USER")==0)
    {
        conf.user=malloc((char)strlen(value)+1);
        strcpy(conf.user, value);
    }
    else if(strcmp(key, "MQTT_PASSWORD")==0)
    {
        conf.password=malloc((char)strlen(value)+1);
        strcpy(conf.password, value);
    }
    else if(strcmp(key, "MQTT_PORT")==0)
    {
        errno=0;
        nvalue=strtol(value, NULL, 10);
        if(errno==0)
            conf.port=nvalue;
    }
    else if(strcmp(key, "MQTT_KEEPALIVE")==0)
    {
        errno=0;
        nvalue=strtol(value, NULL, 10);
        if(errno==0)
            conf.keepalive=nvalue;
    }
    else if(strcmp(key, "MQTT_QOS")==0)
    {
        errno=0;
        nvalue=strtol(value, NULL, 10);
        if(errno==0)
            conf.qos=nvalue;
    }
    else if(strcmp(key, "MQTT_RETAIN")==0)
    {
        errno=0;
        nvalue=strtol(value, NULL, 10);
        if(errno==0)
            conf.retain=nvalue;
    }
    else if(strcmp(key, "MQTT_PREFIX")==0)
    {
        mqttv4_conf.mqtt_prefix=malloc((char)strlen(value)+1);
        strcpy(mqttv4_conf.mqtt_prefix, value);
    }
    else if(strcmp(key, "TOPIC_MOTION")==0)
    {
        mqttv4_conf.topic_motion=malloc((char)strlen(value)+1);
        strcpy(mqttv4_conf.topic_motion, value);
    }
    else if(strcmp(key, "MOTION_START_MSG")==0)
    {
        mqttv4_conf.motion_start_msg=malloc((char)strlen(value)+1);
        strcpy(mqttv4_conf.motion_start_msg, value);
    }
    else if(strcmp(key, "MOTION_STOP_MSG")==0)
    {
        mqttv4_conf.motion_stop_msg=malloc((char)strlen(value)+1);
        strcpy(mqttv4_conf.motion_stop_msg, value);
    }
    else
    {
        printf("key: %s | value: %s\n", key, value);
        fprintf(stderr, "Unrecognized config.\n");
    }
}

static void init_mqttv4_config()
{
    if(init_config(MQTTV4_CONF_FILE)!=0)
    {
        printf("Cannot open config file. Skipping.\n");
        return;
    }

    config_set_handler(&handle_config);
    config_parse();
}
