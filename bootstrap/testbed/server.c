#include <stdio.h>
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

		while (msgpipe_poll(pipe, &msg)) {
			printf("Recieved msg %i\n", msg.type);
		}

		sleep(1);
	}

	msgpipe_close(pipe);

	return 1;
}