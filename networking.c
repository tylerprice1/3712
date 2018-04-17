#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "networking.h"

int robustRead (int fd, void * des, unsigned int bytes) {
	int bytes_read = 0;
	int err = 0;
	if (des != NULL && fd >= 0) {
		unsigned int bytes_left = bytes;
		int eof = 0;
		char * pos = des;
		do {
			bytes_read = read(fd, pos, bytes_left);
			if (bytes_read < 0) {
				/* read error */
				if (errno != EINTR) {
					err = 1;
				}
				/* else interrupted system call */
			}
			/* eof */
			else if (bytes_read == 0) {
				eof = 1;
			}
			/* successful read */
			else {
				bytes_left -= bytes_read;
				pos += bytes_read;
			}
		} while (!err && !eof && bytes_left > 0);
		bytes_read = bytes - bytes_left;
	}
	else {
		err = 1;
	}
	return (err) ? (-1) : (bytes_read);
}
int robustWrite (int fd, const void * src, unsigned int bytes) {
	int bytes_written = 0;
	int err = 0;
	if (src != NULL && fd >= 0) {
		unsigned int bytes_left = bytes;
		const char * pos = src;
		do {
			bytes_written = write(fd, pos, bytes_left);
			if (bytes_written < 0) {
				/* write error */
				if (errno != EINTR) {
					err = 1;
				}
			}
			else {
				bytes_left -= bytes_written;
				pos += bytes_written;
			}
		} while (!err && bytes_left > 0);
		bytes_written = bytes - bytes_left;
	}
	else {
		err = 1;
	}
	return (err) ? (-1) : (bytes_written);
}
