#include <stdlib.h>
#include <string.h>
#include "LinkedListNode.h"

/* LinkedListNode management */
struct LinkedListNode *  LinkedListNode_alloc  (void) {
	return (struct LinkedListNode *)malloc(sizeof(struct LinkedListNode));
}
struct LinkedListNode *  LinkedListNode_init  (struct LinkedListNode * node) {
	if (node != NULL) {
		node->previous = NULL;
		node->next = NULL;
		node->data = NULL;
	}
	return node;
}
struct LinkedListNode *  LinkedListNode_initWithData  (struct LinkedListNode * node, const void * data, unsigned long size) {
	if (node != NULL) {
		LinkedListNode_setData(LinkedListNode_init(node), data, size);
		return node;
	}
	return NULL;
}
void  LinkedListNode_destroy  (struct LinkedListNode * node) {
	if (node != NULL) {
		node->previous = NULL;
		node->next = NULL;
		if (node->data != NULL) {
			free(node->data);
			node->data = NULL;
		}
	}
}
void  LinkedListNode_dealloc  (struct LinkedListNode * node) {
	if (node != NULL) {
		LinkedListNode_destroy(node);
		free(node);
	}
}
/* set */
void  LinkedListNode_set  (struct LinkedListNode * node, struct LinkedListNode * p, struct LinkedListNode * n, const void * data, unsigned long size) {
	if (node != NULL) {
		node->previous = p;
		node->next = n;
		LinkedListNode_setData(node, data, size);
	}
}
void  LinkedListNode_setData  (struct LinkedListNode * node, const void * data, unsigned long size) {
	if (node != NULL) {
		if (data != NULL) {
			node->data = (node->data == NULL) ? (malloc(size)) : (realloc(node->data, size));
			if (node->data != NULL)
				memcpy(node->data, data, size);
		} else {
			if (node->data != NULL)
				free(node->data);
		}
	}
}
