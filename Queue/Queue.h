#ifndef Queue_h
#define Queue_h

#include "LinkedList.h"

typedef LinkedList Queue;
/* function pointers */
extern
 Queue *  (* const Queue_alloc)       (void);
extern
 Queue *  (* const Queue_init)        (Queue *);
extern
 Queue *  (* const Queue_initAndSet)  (Queue *, unsigned long, int (*)(const void *, const void *), void (*)(const void *));
extern
 void     (* const Queue_destroy)     (Queue *);
extern
 void     (* const Queue_dealloc)     (Queue *);

extern
 void *  (* const Queue_enqueue)  (Queue *, const void *);
extern
 void    (* const Queue_dequeue)  (Queue *);

extern
 void *  (* const Queue_next)     (Queue *);
extern
 int     (* const Queue_isEmpty)  (const Queue *);

#endif /* Queue_h */
