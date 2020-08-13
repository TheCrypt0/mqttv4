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

#ifndef IPC_H
#define IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <mqueue.h>

#define IPC_LIBRARY_VERSION     "0.1.0"

#define IPC_DEBUG               0

#define IPC_QUEUE_NAME          "/ipc_dispatch_1"
#define IPC_MESSAGE_MAX_SIZE    512

#define IPC_AI_HUMAN_DETECTION_START    "\x01\x00\x00\x00\x02\x00\x00\x00\xf5\x00\xf5\x00\x00\x00\x00\x00"
#define IPC_AI_HUMAN_DETECTION_STOP     "\x01\x00\x00\x00\x02\x00\x00\x00\xed\x00\xed\x00\x00\x00\x00\x00"
#define IPC_MOTION_START                "\x4d\x4f\x54\x49\x4f\x4e\x5f\x53\x54\x41\x52\x54"
#define IPC_MOTION_STOP                 "\x4d\x4f\x54\x49\x4f\x4e\x5f\x53\x54\x4f\x50"
#define IPC_BABY_CRYING                 "\x42\x41\x42\x59\x5f\x43\x52\x59\x49\x4e\x47"

typedef enum
{
    IPC_MSG_UNRECOGNIZED,
    IPC_MSG_AI_HUMAN_DETECTION_START,
    IPC_MSG_AI_HUMAN_DETECTION_STOP,
    IPC_MSG_MOTION_START,
    IPC_MSG_MOTION_STOP,
    IPC_MSG_BABY_CRYING,
    IPC_MSG_LAST
} IPC_MESSAGE_TYPE;

//-----------------------------------------------------------------------------
// INIT
//-----------------------------------------------------------------------------

int ipc_init();
void ipc_stop();

//-----------------------------------------------------------------------------
// GETTERS AND SETTERS
//-----------------------------------------------------------------------------

int ipc_set_callback(IPC_MESSAGE_TYPE type, void (*f)());


#endif // IPC_H
