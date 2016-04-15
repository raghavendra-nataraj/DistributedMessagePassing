/*
 * This file contains functions declarations for manipulating mailboxes.
 */

#ifndef MAILBOX_H
#define MAILBOX_H

#include <pthread.h>

#include "queue.h"

#define NEW_MSG	malloc(sizeof(message));
extern pthread_mutex_t queuelock;
typedef struct mailbox_s {
	queue q;
} mailbox;

enum mtypes {
	ID,
	NID,
	MAILBOX,
	INIPROBE,
	PROBE,
	INISORT,
	SORT,
	PING,
	PRINT,
	SHUTDOWN
};

typedef union {
	int integer;
	mailbox* mb;
} msg_data;

typedef struct {
	enum mtypes type;
	msg_data payload;
} message;

void mailbox_init(mailbox* mb);
message* mailbox_receive(mailbox* mb);
void mailbox_send(mailbox* mb, message* msg);
extern mailbox* first_mb;
#endif
