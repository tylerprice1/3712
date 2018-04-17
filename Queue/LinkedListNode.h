#ifndef LinkedListNode_h
#define LinkedListNode_h

struct LinkedListNode {
	struct LinkedListNode * previous;
	struct LinkedListNode * next;
	void * data;
};

/* LinkedListNode management */
struct LinkedListNode *  LinkedListNode_alloc         (void);
struct LinkedListNode *  LinkedListNode_init          (struct LinkedListNode *);
struct LinkedListNode *  LinkedListNode_initWithData  (struct LinkedListNode *, const void *, unsigned long);
void                     LinkedListNode_destroy       (struct LinkedListNode *);
void                     LinkedListNode_dealloc       (struct LinkedListNode *);
/* get */
#define LinkedListNode_dereferenceDataAsType(lln, type)  (*((type)*)((lln)->data))
#define LinkedListNode_getDataAsPointerToType(lln, type) ( ((type)*)((lln)->data))
/* set */
void  LinkedListNode_set      (struct LinkedListNode *, struct LinkedListNode *, struct LinkedListNode *, const void *, unsigned long);
void  LinkedListNode_setData  (struct LinkedListNode *, const void *, unsigned long);

#endif /* LinkedListNode_h */
