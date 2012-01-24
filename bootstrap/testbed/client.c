#include <stdio.h>
#include <unistd.h>
#include "../msgpipe.h"

#define FIFO_NAME "q3pluggedin"

int main() {
	msgpipe* pipe = msgpipe_open(FIFO_NAME);

	if (!pipe) {
		fprintf(stderr, "Failed to make event pipe.\n");
		return 1;
	}

	while (1) {
		msgpipe_msg msg;
		msg.type = KEYPRESS;
		msg.keypress.pressing = 1;
		msg.keypress.key = 100;

		msgpipe_send(pipe, &msg);

		sleep(5);
	}

	msgpipe_close(pipe);

	return 0;
}