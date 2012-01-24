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

msgpipe* msgpipe_open(const char* name) {
	int ret;

	errno = -1;
	ret = mkfifo(name, 0666);

	if (ret < 0 && errno != EEXIST) {
		fprintf(stderr, "Failed to make pipe '%s'.\n", name);
		return NULL;
	}

	msgpipe* pipe = (msgpipe*)malloc(sizeof(msgpipe));
	memset(pipe, 0, sizeof(msgpipe));
	snprintf(pipe->name, sizeof(pipe->name), "%s", name);
	pipe->fd = -1;

	return pipe;
}

void msgpipe_close(msgpipe* pipe) {
	close(pipe->fd);
}

void msgpipe_send(msgpipe* pipe, msgpipe_msg* msg) {
	int num;

	if (pipe->fd < 0 &&
		(pipe->fd = open(pipe->name, O_WRONLY)) < 0) {
		fprintf(stderr, "Failed to open pipe '%s'.\n", pipe->name);
		return;
	}

	if ((num = write(pipe->fd, msg, sizeof(msgpipe_msg)) < 0)) {
		fprintf(stderr, "Failed to write to pipe.\n");
		return;
	}
}

void msgpipe_pump(msgpipe* pipe) {
	int num;

	if (pipe->fd < 0 &&
		(pipe->fd = open(pipe->name, O_RDONLY | O_NONBLOCK)) < 0) {
		fprintf(stderr, "Failed to open pipe '%s'.\n", pipe->name);
		return;
	}

	// We don't want to read more than our current buffer can hold.
	char* buf = pipe->buf + pipe->buflen;
	int length = sizeof(char) * sizeof(pipe->buf) - pipe->buflen;

	errno = -1;
	num = read(pipe->fd, buf, length);

	if (num == -1 && errno != EAGAIN) {
		fprintf(stderr, "Failed to read from pipe, errno %i.", errno);
		return;
	} else if (num > 0) {
		// Update our current buffer size on a successful read.
		pipe->buflen += num;
	}
}

int msgpipe_poll(msgpipe* pipe, msgpipe_msg* msg) {
	// Pump data in from the pipe.
	msgpipe_pump(pipe);

	// Parse the data we pumped in.
	while (1) {
		if (pipe->buflen >= sizeof(msgpipe_msg)) {
			// Copy message from buffer to input param.
			memcpy(msg, pipe->buf, sizeof(msgpipe_msg));

			// Remove message from buffer.
			pipe->buflen -= sizeof(msgpipe_msg);
			memcpy(pipe->buf, pipe->buf + sizeof(msgpipe_msg), pipe->buflen);

			return 1;
		}

		return 0;
	}
}