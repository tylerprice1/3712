#include <stdlib.h>
#include "LinkedList.h"
#include "LinkedListNode.h"

/* LinkedList management */
struct LinkedList *  LinkedList_alloc  (void) {
	return (struct LinkedList *)malloc(sizeof(struct LinkedList));
}
struct LinkedList *  LinkedList_init  (struct LinkedList * ll) {
	if (ll != NULL) {
		ll->head = NULL;
		ll->tail = NULL;
		ll->compare = NULL;
		ll->print = NULL;
		ll->size = 0;
	}
	return ll;
}

struct LinkedList *  LinkedList_initAndSet  (struct LinkedList * ll, unsigned long size, int (*compare)(const void *, const void *), void (*print)(const void *)) {
	if (ll != NULL) {
		ll->head = NULL;
		ll->tail = NULL;
		ll->compare = compare;
		ll->print = print;
		ll->size = size;
	}
	return ll;
}

void  LinkedList_destroy  (struct LinkedList * ll) {
	if (ll != NULL) {
		while (ll->head != NULL) {
			LinkedList_popFront(ll);
		}
		LinkedList_init(ll);
	}
}

void  LinkedList_dealloc  (struct LinkedList * ll) {
	if (ll != NULL) {
		while (ll->head != NULL) {
			LinkedList_popFront(ll);
		}
		free(ll);
	}
}
unsigned long  LinkedList_getSize  (const struct LinkedList * ll) {
	const struct LinkedListNode * curr;
	unsigned long size = 0;
	if (ll != NULL) {
		curr = ll->head;
		while (curr != NULL) {
			size++;
			curr = curr->next;
		}
	}
	return size;
}
/* pushfront */
void *  LinkedList_pushFront  (struct LinkedList * ll, const void * data) {
	struct LinkedListNode * newnode;
	if (ll != NULL) {
		newnode = LinkedListNode_initWithData(LinkedListNode_alloc(), data, ll->size);
		if (newnode != NULL) {
			if (LinkedList_pushFrontNode(ll, newnode) != NULL)
				return newnode->data;
			else
				LinkedListNode_dealloc(newnode);
		}
	}
	return NULL;
}
struct LinkedListNode *  LinkedList_pushFrontNode  (struct LinkedList * ll, struct LinkedListNode * node) {
	if (ll != NULL  &&  node != NULL) {
		if (ll->head == NULL) {
			ll->head = node;
			ll->tail = node;
		} else {
			node->previous = NULL;
			node->next = ll->head;
			ll->head->previous = node;
			ll->head = node;
		}
		return node;
	}
	return NULL;
}
/* pushback */
void *  LinkedList_pushBack  (struct LinkedList * ll, const void * data) {
	struct LinkedListNode * newnode;
	if (ll != NULL) {
		newnode = LinkedListNode_initWithData(LinkedListNode_alloc(), data, ll->size);
		if (newnode != NULL) {
			if (LinkedList_pushBackNode(ll, newnode) != NULL) {
				return newnode->data;
			} else {
				LinkedListNode_dealloc(newnode);
			}
		}
	}
	return NULL;
}
struct LinkedListNode *  LinkedList_pushBackNode  (struct LinkedList * ll, struct LinkedListNode * node) {
	if (ll != NULL  &&  node != NULL) {
		if (ll->head == NULL) {
			ll->head = node;
			ll->tail = node;
		} else {
			node->previous = ll->tail;
			ll->tail->next = node;
			ll->tail = node;
		}
		return node;
	}
	return NULL;
}
/* pop */
void  LinkedList_popFront  (struct LinkedList * ll) {
	struct LinkedListNode * popped;
	if (ll != NULL  &&  ll->head != NULL) {
		popped = ll->head;
		if (ll->head == ll->tail) {
			ll->head = NULL;
			ll->tail = NULL;
		} else {
			ll->head = ll->head->next;
		}
		LinkedListNode_dealloc(popped);
	}
}
void  LinkedList_popBack  (struct LinkedList * ll) {
	struct LinkedListNode * popped;
	if (ll != NULL  &&  ll->tail != NULL) {
		popped = ll->tail;
		if (ll->head == ll->tail) {
			ll->head = NULL;
			ll->tail = NULL;
		} else {
			ll->tail = ll->tail->previous;
		}
		LinkedListNode_dealloc(popped);
	}
}
/* get */
void *  LinkedList_getFront  (struct LinkedList * ll) {
	return (ll != NULL  &&  ll->head != NULL) ? (ll->head->data) : (NULL);
}
void *  LinkedList_getBack  (struct LinkedList * ll) {
	return (ll != NULL  &&  ll->tail != NULL) ? (ll->tail->data) : (NULL);
}
/* insert before */
void *  LinkedList_insertBefore  (struct LinkedList * ll, const void * insert, const void * before) {
	struct LinkedListNode * insertNode = LinkedList_insertDataBeforeNode(ll, insert, LinkedList_findNodeWithData(ll, before));
	return (insertNode != NULL) ? (insertNode->data) : (NULL);
}
struct LinkedListNode *  LinkedList_insertDataBeforeNode  (struct LinkedList * ll, const void * insert, struct LinkedListNode * beforeNode) {
	struct LinkedListNode * insertNode;
	if (ll != NULL) {
		insertNode = LinkedListNode_initWithData(LinkedListNode_alloc(), insert, ll->size);
		if (LinkedList_insertNodeBeforeNode(ll, insertNode, beforeNode) != NULL)
			return insertNode;
		else
			LinkedListNode_dealloc(insertNode);
	}
	return NULL;
}
struct LinkedListNode *  LinkedList_insertNodeBeforeNode  (struct LinkedList * ll, struct LinkedListNode * insertNode, struct LinkedListNode * beforeNode) {
	if (ll != NULL  &&  insertNode != NULL  &&  beforeNode != NULL) {
		insertNode->next = beforeNode;
		insertNode->previous = beforeNode->previous;
		beforeNode->previous = insertNode;
		if (ll->head == beforeNode)
			ll->head = insertNode;
		return insertNode;
	}
	return NULL;
}
/* insert after */
void *  LinkedList_insertAfter  (struct LinkedList * ll, const void * insert, const void * after) {
	struct LinkedListNode * insertNode = LinkedList_insertDataAfterNode(ll, insert, LinkedList_findNodeWithData(ll, after));
	return (insertNode != NULL) ? (insertNode->data) : (NULL);
}
struct LinkedListNode *  LinkedList_insertDataAfterNode  (struct LinkedList * ll, const void * insert, struct LinkedListNode * afterNode) {
	struct LinkedListNode * insertNode;
	if (ll != NULL) {
		insertNode = LinkedListNode_initWithData(LinkedListNode_alloc(), insert, ll->size);
		if (LinkedList_insertNodeAfterNode(ll, insertNode, afterNode) != NULL)
			return insertNode;
		else
			LinkedListNode_dealloc(insertNode);
	}
	return NULL;
}
struct LinkedListNode *  LinkedList_insertNodeAfterNode  (struct LinkedList * ll, struct LinkedListNode * insertNode, struct LinkedListNode * afterNode) {
	if (ll != NULL  &&  insertNode != NULL  &&  afterNode != NULL) {
		insertNode->previous = afterNode;
		insertNode->next = afterNode->next;
		afterNode->next = insertNode;
		if (ll->tail == afterNode)
			ll->tail = insertNode;
		return insertNode;
	}
	return NULL;
}
/* removal */
void  LinkedList_remove  (struct LinkedList * ll, const void * remove) {
	LinkedList_removeNode(ll, LinkedList_findNodeWithData(ll, remove));
}
void  LinkedList_removeNode  (struct LinkedList * ll, struct LinkedListNode * removeNode) {
	if (ll != NULL  &&  removeNode != NULL) {
		if (ll->head == removeNode)
			ll->head = removeNode->next;
		else
			removeNode->previous->next = removeNode->next;
		if (ll->tail == removeNode)
			ll->tail = removeNode->previous;
		else
			removeNode->next->previous = removeNode->previous;
		LinkedListNode_dealloc(removeNode);
	}
}
/* search */
void *  LinkedList_search  (struct LinkedList * ll, const void * key) {
	struct LinkedListNode * result = LinkedList_findNodeWithData(ll, key);
	return (result != NULL) ? (result->data) : (NULL);
}
struct LinkedListNode *  LinkedList_findNodeWithData(struct LinkedList * ll, const void * key) {
	struct LinkedListNode * curr;
	if (ll != NULL  &&  ll->compare  &&  key != NULL) {
		curr = ll->head;
		while (curr != NULL) {
			if (ll->compare(curr->data, key) == 0)
				return curr;
			curr = curr->next;
		}
	}
	return NULL;
}
/* others */
int  LinkedList_isEmpty  (const struct LinkedList * ll) {
	return (ll != NULL) ? (ll->head == NULL) : 1;
}
void  LinkedList_print  (const struct LinkedList * ll) {
	struct LinkedListNode * curr;
	if (ll != NULL  &&  ll->print != NULL) {
		curr = ll->head;
		while (curr != NULL) {
			ll->print(curr->data);
			curr = curr->next;
		}
	}
}
