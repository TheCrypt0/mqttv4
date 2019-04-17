#include "mqttv4.h"

void callback_motion_start()
{
    printf("CALLBACK MOTION START\n");
}

void callback_motion_stop()
{
    printf("CALLBACK MOTION STOP\n");
}

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

    ret=ipc_init();
    if(ret!=0)
        exit(EXIT_FAILURE);

    ipc_set_callback(IPC_MSG_MOTION_START, &callback_motion_start);
    ipc_set_callback(IPC_MSG_MOTION_STOP, &callback_motion_stop);

    while(1)
    {
        mqtt_msg_t msg;
        char test[]="TEST_123";
        char topic[]="test/1";

        msg.msg=test;
        msg.len=strlen(test);
        msg.topic=topic;

        //printf("Sending message...\n");

        mqtt_send_message(&msg);

        mqtt_loop();

        usleep(500*1000);
    }

    return 0;
}
