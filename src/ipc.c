/*
 * This file is part of libipc (https://github.com/TheCrypt0/libipc).
 * Copyright (c) 2019 Davide Maggioni.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ipc.h"

//-----------------------------------------------------------------------------
// GENERAL STATIC VARS AND FUNCTIONS
//-----------------------------------------------------------------------------

static mqd_t ipc_mq;
static pthread_t *tr_queue;
int tr_queue_routine;

static int open_queue();
static int clear_queue();
static int start_queue_thread();
static void *queue_thread(void *args);
static int parse_message(char *msg, ssize_t len);

static void call_callback(IPC_MESSAGE_TYPE type);
static void ipc_debug(const char* fmt, ...);

//-----------------------------------------------------------------------------
// MESSAGES HANDLERS
//-----------------------------------------------------------------------------

static void handle_ipc_motion_start();
static void handle_ipc_motion_stop();
static void handle_ipc_baby_crying();

static void handle_ipc_unrecognized();

//-----------------------------------------------------------------------------
// FUNCTION POINTERS TO CALLBACKS
//-----------------------------------------------------------------------------

typedef void(*func_ptr_t)(void);

static func_ptr_t *ipc_callbacks;

//=============================================================================

//-----------------------------------------------------------------------------
// INIT
//-----------------------------------------------------------------------------

int ipc_init()
{
    int ret;

    ret = open_queue();
    if(ret != 0)
        return -1;

    ret = clear_queue();
    if(ret != 0)
        return -2;

    ipc_callbacks=malloc((sizeof(func_ptr_t))*IPC_MSG_LAST);

    ret=start_queue_thread();
    if(ret!=0)
        return -2;

    return 0;
}

void ipc_stop()
{
    if(tr_queue!=NULL)
    {
        tr_queue_routine=0;
        pthread_join((*tr_queue), NULL);
        free(tr_queue);
    }

    if(ipc_callbacks!=NULL)
        free(ipc_callbacks);

    if(ipc_mq>0)
        mq_close(ipc_mq);
}

//-----------------------------------------------------------------------------
// MQ_QUEUE STUFF
//-----------------------------------------------------------------------------

static int open_queue()
{
    ipc_mq=mq_open(IPC_QUEUE_NAME, O_RDWR);
    if(ipc_mq==-1)
    {
        fprintf(stderr, "Can't open mqueue %s. Error: %s\n", IPC_QUEUE_NAME,
                strerror(errno));
        return -1;
    }
    return 0;
}

static int clear_queue()
{
    struct mq_attr attr;
    char buffer[IPC_MESSAGE_MAX_SIZE + 1];

    if (mq_getattr(ipc_mq, &attr) == -1) {
        fprintf(stderr, "Can't get queue attributes\n");
        return -1;
    }

    while (attr.mq_curmsgs > 0) {
        ipc_debug("Clear message in queue...");
        mq_receive(ipc_mq, buffer, IPC_MESSAGE_MAX_SIZE, NULL);
        ipc_debug(" done.\n");
        if (mq_getattr(ipc_mq, &attr) == -1) {
            fprintf(stderr, "Can't get queue attributes\n");
            return -1;
        }
    }

    return 0;
}

static int start_queue_thread()
{
    int ret;

    tr_queue=malloc(sizeof(pthread_t));
    tr_queue_routine=1;
    ret=pthread_create(tr_queue, NULL, &queue_thread, NULL);
    if(ret!=0)
    {
        fprintf(stderr, "Can't create ipc thread. Error: %d\n", ret);
        return -1;
    }

    return 0;
}

static void *queue_thread(void *args)
{
    ssize_t bytes_read;
    char buffer[IPC_MESSAGE_MAX_SIZE];

    while(tr_queue_routine)
    {
        bytes_read=mq_receive(ipc_mq, buffer, IPC_MESSAGE_MAX_SIZE, NULL);

        ipc_debug("IPC message. Len: %d. Status: %s!\n", bytes_read,
                  strerror(errno));

        if(bytes_read>=0)
        {
            parse_message(buffer, bytes_read);
        }
        usleep(10*1000);
    }

    return 0;
}

//-----------------------------------------------------------------------------
// IPC PARSER
//-----------------------------------------------------------------------------

static int parse_message(char *msg, ssize_t len)
{
    int i;
    ipc_debug("Parsing message.\n");

    for(i=0; i<len; i++)
        ipc_debug("%02x ", msg[i]);
    ipc_debug("\n");

    if((len >= sizeof(IPC_MOTION_START) - 1) && (memcmp(msg, IPC_MOTION_START, sizeof(IPC_MOTION_START) - 1)==0))
    {
        handle_ipc_motion_start();
        return 0;
    }
    else if((len >= sizeof(IPC_MOTION_STOP) - 1) && (memcmp(msg, IPC_MOTION_STOP, sizeof(IPC_MOTION_STOP) - 1)==0))
    {
        handle_ipc_motion_stop();
        return 0;
    }
    else if((len >= sizeof(IPC_BABY_CRYING) - 1) && (memcmp(msg, IPC_BABY_CRYING, sizeof(IPC_BABY_CRYING) - 1)==0))
    {
        handle_ipc_baby_crying();
        return 0;
    }

    handle_ipc_unrecognized();

    return -1;
}

//-----------------------------------------------------------------------------
// IPC HANDLERS
//-----------------------------------------------------------------------------

static void handle_ipc_unrecognized()
{
    ipc_debug("GOT UNRECOGNIZED MESSAGE\n");
//    call_callback(IPC_MSG_UNRECOGNIZED);
}

static void handle_ipc_motion_start()
{
    ipc_debug("GOT MOTION START\n");
    call_callback(IPC_MSG_MOTION_START);
}

static void handle_ipc_motion_stop()
{
    ipc_debug("GOT MOTION STOP\n");
    call_callback(IPC_MSG_MOTION_STOP);
}

static void handle_ipc_baby_crying()
{
    ipc_debug("GOT BABY CRYING\n");
    call_callback(IPC_MSG_BABY_CRYING);
}

//-----------------------------------------------------------------------------
// GETTERS AND SETTERS
//-----------------------------------------------------------------------------

int ipc_set_callback(IPC_MESSAGE_TYPE type, void (*f)())
{
    if(type>=IPC_MSG_LAST)
        return -1;

    ipc_callbacks[(int)type]=f;

    return 0;
}

//-----------------------------------------------------------------------------
// UTILS
//-----------------------------------------------------------------------------

static void call_callback(IPC_MESSAGE_TYPE type)
{
    func_ptr_t f;
    // Not handling callbacks with parameters (yet)
    f=ipc_callbacks[(int)type];
    if(f!=NULL)
        (*f)();
}

static void ipc_debug(const char* fmt, ...)
{
#if IPC_DEBUG
    va_list args;
    va_start (args, fmt);
    vprintf(fmt, args);
    va_end (args);
#endif
}
