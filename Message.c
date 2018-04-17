#include <string.h>
#include <stdlib.h>
#include "Message.h"

struct Message * message_deepcopy(struct Message * destination, const struct Message * source) {
	if (destination != NULL && source != NULL) {
		char * temp;
		if (destination->text != NULL) {
			temp = (char *)realloc(destination->text, source->length * sizeof(char));
		}
		else {
			temp = (char *)malloc(source->length * sizeof(char));
		}
		if (temp != NULL) {
			destination->length = source->length;
			destination->text = temp;
			strcpy(destination->text, source->text);
			return destination;
		}
	}
	return NULL;
}
