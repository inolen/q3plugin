#ifndef MSGPIPE_H
#define MSGPIPE_H

typedef enum {
	PIPE_READ,
	PIPE_WRITE
} PIPE_END;

typedef enum {
	KEYPRESS,
	MOUSEPRESS,
	MOUSEMOTION
} MESSAGE_TYPE;

typedef enum {
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT
} MOUSE_BUTTON;

typedef struct msg_keypress {
	int type;
	int pressing;
	int key;
} msg_keypress;

typedef struct msg_mousepress {
	int type;
	int pressing;
	int button;
} msg_mousepress;

typedef struct msg_mousemotion {
	int type;
	int xrel;
	int yrel;
} msg_mousemotion;

typedef union msgpipe_msg {
	int type;
	msg_keypress keypress;
	msg_mousepress mousepress;
	msg_mousemotion mousemotion;
} msgpipe_msg;

typedef struct msgpipe_s {
	char name[128];
	int fd;

	// This needs to be larger than sizeof(msgpipe_msg)
	char buf[1024];
	int buflen;
} msgpipe;

extern msgpipe* msgpipe_open(const char* name, PIPE_END type);
extern void msgpipe_close(msgpipe* pipe);
extern void msgpipe_send(msgpipe* pipe, msgpipe_msg* msg);
extern int msgpipe_poll(msgpipe* pipe, msgpipe_msg* msg);

#endif