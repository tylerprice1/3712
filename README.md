# 3712
Project 2

Some documentation

Queue/:
	This directory contains files implementing a Linked List and a Queue.
	The header contains functions and the data structures used.
	The Queue/ is type-generic: it uses void pointers for data storage.

	E.g., the return value of Queue_next is a void pointer to the
	data at the front of the queue. The value is NULL if the Queue is empty.
	The item can be typecasted and dereferenced to obtain a copy of the first item.
	"int next = *(int *)Queue_next(anInstanceOfQueue);"
	This allows for seg. faults if the Queue is empty.
	"
	int next;
	if ( !Queue_isEmpty(anInstanceOfQueue) )
		next = *(int *)Queue_next(anInstanceOfQueue);
	"
	This is better.
	
mycloud/:
	mycloud contains old code from systems that contains
	a good outline of the process of setting up a connection
	between client and server. It's intended as a reference
	from which code can be more or less rewritten into this
	project.
