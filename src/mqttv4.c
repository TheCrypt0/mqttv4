#include "mqttv4.h"

int main(int argc, char **argv)
{
    int ret;

    mqtt_conf_t conf;

    ret=init_mqtt();
    if(ret!=0)
        exit(EXIT_FAILURE);

    mqtt_init_conf(&conf);

    strcpy(conf.host, "server-test");

    mqtt_set_conf(&conf);

    ret=mqtt_connect();
    if(ret!=0)
        exit(EXIT_FAILURE);

    while(1)
    {
        mqtt_msg_t msg;
        char test[]="TEST_123";
        char topic[]="test/1";

        msg.msg=test;
        msg.len=strlen(test);
        msg.topic=topic;

        printf("Sending message...\n");

        mqtt_send_message(&msg);

        mqtt_loop();

        usleep(500*1000);
    }

    return 0;
}
