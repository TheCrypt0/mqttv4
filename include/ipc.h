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
#include <errno.h>
#include <pthread.h>
#include <mqueue.h>

#define IPC_DEBUG               0

#define IPC_QUEUE_NAME          "/ipc_dispatch"
#define IPC_MESSAGE_MAX_SIZE    512

#define IPC_FIRST_HEADER_0      "\x01\x00\x00\x00"

#define IPC_SECOND_HEADER_0     "\x02\x00\x00\x00"

#define IPC_MOTION_START        "\x7c\x00\x7c\x00"
#define IPC_MOTION_STOP         "\x7d\x00\x7d\x00"

typedef enum
{
    IPC_MSG_UNRECOGNIZED,
    IPC_MSG_MOTION_START,
    IPC_MSG_MOTION_STOP,
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
