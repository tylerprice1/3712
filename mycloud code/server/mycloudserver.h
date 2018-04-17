#ifndef mycloudserver_h
#define mycloudserver_h

#include "mycloud.h"

int mycloudserver_listen (const char * port);

int   mycloudserver_readRequest     (int, struct mycloud_request *);
int   mycloudserver_writeResponseToRequest   (int, const struct mycloud_request *, const struct mycloud_response *);
void  mycloudserver_processRequest  (struct mycloud_request *, struct mycloud_response *);

void  mycloudserver_retrieve  (const char [MAX_FILENAME], struct mycloud_response *);
void  mycloudserver_delete    (const char [MAX_FILENAME], struct mycloud_response *);
void  mycloudserver_store     (const char [MAX_FILENAME], const void *, unsigned int, struct mycloud_response *);
void  mycloudserver_list      (struct mycloud_response *);

#endif
