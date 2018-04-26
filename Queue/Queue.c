
#include "Queue.h"

Queue *  (* const Queue_alloc)       (void)     = &LinkedList_alloc;
Queue *  (* const Queue_init)        (Queue *)  = &LinkedList_init;
Queue *  (* const Queue_initAndSet)  (Queue *, unsigned long, int (*)(const void *, const void *), void (*)(const void *))  = &LinkedList_initAndSet;
void     (* const Queue_destroy)     (Queue *)  = &LinkedList_destroy;
void     (* const Queue_dealloc)     (Queue *)  = &LinkedList_dealloc;

void *  (* const Queue_enqueue)  (Queue *, const void *)  = &LinkedList_pushBack;
void    (* const Queue_dequeue)  (Queue *)                = &LinkedList_popFront;

void *  (* const Queue_next)     (Queue *)  = &LinkedList_getFront;
int     (* const Queue_isEmpty)  (const Queue *)  = &LinkedList_isEmpty;
