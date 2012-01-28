#include "msgpipe.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern int errno;

static int _msgpipe_open_read(msgpipe* pipe) {
	errno = 0;
	if (mkfifo(pipe->namer, 0666) < 0 && errno != EEXIST) {
		return -1;
	}

	if ((pipe->fdr = open(pipe->namer, O_RDONLY)) < 0) {
		return -1;
	}

	// After we've successfully opened, set the read pipe to non-blocking.
	fcntl(pipe->fdr, F_SETFL, O_RDONLY | O_NONBLOCK);

	return 0;
}

static int _msgpipe_open_write(msgpipe* pipe) {
	// Create/open write pipe.
	errno = 0;
	if (mkfifo(pipe->namew, 0666) < 0 && errno != EEXIST) {
		return -1;
	}

	if ((pipe->fdw = open(pipe->namew, O_WRONLY)) < 0) {
		return -1;
	}

	return 0;
}

/*
 * Reverse is a bit of a confusing term. Named pipes offer half-duplex communication,
 * so we open two pipes for full-duplex. The reverse flag lets the code open the
 * correct pipe, in the correct order on both sides so we avoid deadlock. One side will
 * need to specify false, one will need to specify true.
 */
int msgpipe_open(msgpipe* pipe, const char *name, bool reverse) {
	if (pipe == NULL || name == NULL) {
		return -1;
	}

	snprintf(pipe->namer, sizeof(pipe->namer), "%s%i", name, reverse);
	snprintf(pipe->namew, sizeof(pipe->namew), "%s%i", name, !reverse);

	if ((!reverse && (_msgpipe_open_read(pipe) < 0 || _msgpipe_open_write(pipe) < 0)) ||
	    (reverse && (_msgpipe_open_write(pipe) < 0 || _msgpipe_open_read(pipe) < 0))) {
		msgpipe_close(pipe);
		return -1;
	}

	pipe->buflen = 0;

	return 0;
}

void msgpipe_close(msgpipe* pipe) {
	if (pipe == NULL) {
		return;
	}

	close(pipe->fdr);
	close(pipe->fdw);
}

int msgpipe_send(msgpipe* pipe, msgpipe_msg* msg) {
	int num;

	if (pipe == NULL || msg == NULL) {
		return -1;
	}

	if ((num = write(pipe->fdw, msg, sizeof(msgpipe_msg)) < 0)) {
		return -1;
	}

	return num;
}

void msgpipe_pump(msgpipe* pipe) {
	int num;

	if (pipe == NULL) {
		return;
	}

	// We don't want to read more than our current buffer can hold.
	char* buf = pipe->buf + pipe->buflen;
	int length = sizeof(char) * sizeof(pipe->buf) - pipe->buflen;

	if ((num = read(pipe->fdr, buf, length)) == -1 && errno != EAGAIN) {
		return;
	} else if (num > 0) {
		// Update our current buffer size on a successful read.
		pipe->buflen += num;
	}
}

int msgpipe_poll(msgpipe* pipe, msgpipe_msg* msg) {
	if (pipe == NULL || msg == NULL) {
		return 0;
	}

	// Pump data in from the pipe.
	msgpipe_pump(pipe);

	if (pipe->buflen < sizeof(msgpipe_msg)) {
		return 0;
	}

	// Copy message from buffer to input param.
	memcpy(msg, pipe->buf, sizeof(msgpipe_msg));

	// Remove message from buffer.
	pipe->buflen -= sizeof(msgpipe_msg);
	memcpy(pipe->buf, pipe->buf + sizeof(msgpipe_msg), pipe->buflen);

	return 1;
}