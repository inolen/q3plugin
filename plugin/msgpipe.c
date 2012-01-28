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

msgpipe* msgpipe_open(const char* name, PIPE_END type) {
	int ret, fd;

	if (name == NULL) {
		return NULL;
	}

	errno = 0;
	if ((ret = mkfifo(name, 0666)) < 0 && errno != EEXIST) {
		fprintf(stderr, "Failed to make pipe '%s'.\n", name);
		return NULL;
	}

	if ((fd = open(name, type == PIPE_READ ? O_RDONLY : O_WRONLY)) < 0) {
		fprintf(stderr, "Failed to open pipe '%s'.\n", name);
		return NULL;
	}

	// After we've successfully opened, set the read pipe to non-blocking.
	if (type == PIPE_READ) {
		fcntl(fd, F_SETFL, O_RDONLY | O_NONBLOCK);
	}

	msgpipe* pipe = (msgpipe*)malloc(sizeof(msgpipe));
	memset(pipe, 0, sizeof(msgpipe));
	snprintf(pipe->name, sizeof(pipe->name), "%s", name);
	pipe->fd = fd;

	return pipe;
}

void msgpipe_close(msgpipe* pipe) {
	if (pipe == NULL) {
		return;
	}

	close(pipe->fd);
}

int msgpipe_send(msgpipe* pipe, msgpipe_msg* msg) {
	int num;

	if (pipe == NULL || msg == NULL) {
		return -1;
	}

	if ((num = write(pipe->fd, msg, sizeof(msgpipe_msg)) < 0)) {
		fprintf(stderr, "Failed to write to pipe, errno %i (fd: %i).\n", errno, pipe->fd);
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

	if ((num = read(pipe->fd, buf, length)) == -1 && errno != EAGAIN) {
		fprintf(stderr, "Failed to read from pipe, errno %i.", errno);
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