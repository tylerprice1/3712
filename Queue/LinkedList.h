#ifndef LinkedList_h
#define LinkedList_h

#include "LinkedListNode.h"

struct LinkedList {
	int   (*compare)  (const void *, const void *);
	void  (*print)    (const void *);
	struct LinkedListNode * head;
	struct LinkedListNode * tail;
	unsigned long size; /* size of data in linked list */
};

typedef  struct LinkedList  LinkedList;

/* allocate/initialize/free */
struct LinkedList *  LinkedList_alloc       (void);
struct LinkedList *  LinkedList_init        (struct LinkedList *);
struct LinkedList *  LinkedList_initAndSet  (struct LinkedList *, unsigned long, int (*)(const void *, const void *), void (*)(const void *));
void                 LinkedList_destroy     (struct LinkedList *);
void                 LinkedList_dealloc     (struct LinkedList *);
/* get */
unsigned long  LinkedList_getSize  (const struct LinkedList *); /* returns number of elements currently in list: O(n) */
/* pushFront */
void *                   LinkedList_pushFront      (struct LinkedList *, const void *);
struct LinkedListNode *  LinkedList_pushFrontNode  (struct LinkedList *, struct LinkedListNode *);
/* pushBack */
void *                   LinkedList_pushBack      (struct LinkedList *, const void *);
struct LinkedListNode *  LinkedList_pushBackNode  (struct LinkedList *, struct LinkedListNode *);
/* pop */
void  LinkedList_popFront  (struct LinkedList *);
void  LinkedList_popBack   (struct LinkedList *);
/* get */
void *  LinkedList_getFront  (struct LinkedList *);
void *  LinkedList_getBack   (struct LinkedList *);
/* insertBefore */
void *                   LinkedList_insertBefore          (struct LinkedList *, const void *, const void *);
struct LinkedListNode *  LinkedList_insertDataBeforeNode  (struct LinkedList *, const void *, struct LinkedListNode *);
struct LinkedListNode *  LinkedList_insertNodeBeforeNode  (struct LinkedList *, struct LinkedListNode *, struct LinkedListNode *);
/* insertAfter */
void *                   LinkedList_insertAfter          (struct LinkedList *, const void *, const void *);
struct LinkedListNode *  LinkedList_insertDataAfterNode  (struct LinkedList *, const void *, struct LinkedListNode *);
struct LinkedListNode *  LinkedList_insertNodeAfterNode  (struct LinkedList *, struct LinkedListNode *, struct LinkedListNode *);
/* deletion */
void  LinkedList_remove      (struct LinkedList *, const void *);
void  LinkedList_removeNode  (struct LinkedList *, struct LinkedListNode *);
/* search */
void *                   LinkedList_search            (struct LinkedList *, const void *);
struct LinkedListNode *  LinkedList_findNodeWithData  (struct LinkedList *, const void *);
/* others */
int    LinkedList_isEmpty  (const struct LinkedList *);
void   LinkedList_print    (const struct LinkedList *);

#endif /* LinkedList_h */
