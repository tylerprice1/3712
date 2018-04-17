#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "message.h"

struct MessageQueueNode {
	struct Message message;
	struct MessageQueueNode * prev;
	struct MessageQueueNode * next;
}

struct MessageQueue {
	struct MessageQueueNode * head;
	struct MessageQueueNode * tail;
};

void  MessageQueue_init(struct MessageQueue * queue);
void  MessageQueue_destroy(struct MessageQueue * queue);

void  MessageQueue_enqueue(struct MessageQueue * queue, const struct Message * message);
void  MessageQueue_dequeue(struct MessageQueue * queue, struct Message * message);

int  MessageQueue_isEmpty(const struct MessageQueue * queue);

#endif
