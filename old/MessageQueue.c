
#include <string.h>
#include <stdlib.h>
#include "MessageQueue.h"

void MessageQueue_init(struct MessageQueue * const queue) {
	if (queue != NULL)
		memset(queue, 0, sizeof(struct MessageQueue));
}
void  MessageQueue_destroy(struct MessageQueue * const queue) {
	while ( ! MessageQueue_isEmpty(queue) )
		MessageQueue_dequeue(queue, NULL);
}

int  MessageQueue_enqueue(struct MessageQueue * const queue, const struct Message * const message) {
	if (queue != NULL && message != NULL) {
		struct MessageQueueNode * node;
		node = (struct MessageQueueNode *)malloc(sizeof(struct MessageQueueNode));
		if (node != NULL) {
			Message_deepcopy(node->message, message);
			if (queue->head == NULL) {

			}
			return 0;
		}
	}
	return -1;
}
void  MessageQueue_dequeue(struct MessageQueue * const queue, struct Message * const message) {

}

int  MessageQueue_isEmpty(const struct MessageQueue * const queue) {
	return (queue != NULL) ? (queue->head != NULL) : 1;
}
