#ifndef MSGPIPE_H
#define MSGPIPE_H

#include <stdbool.h>

typedef enum {
	KEYPRESS,
	MOUSEPRESS,
	MOUSEMOTION,
	MOUSELOCK
} MESSAGE_TYPE;

typedef enum {
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT
} MOUSE_BUTTON;

// Plugin -> Shim
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

// Shim -> Plugin
typedef struct msg_mouselock {
	int type;
	bool lock;
} msg_mouselock;

typedef union msgpipe_msg {
	int type;
	msg_keypress keypress;
	msg_mousepress mousepress;
	msg_mousemotion mousemotion;
	msg_mouselock mouselock;
} msgpipe_msg;

typedef struct msgpipe_s {
	char namer[128];
	char namew[128];

	// Set inside of open.
	int fdr;
	int fdw;

	// This needs to be larger than sizeof(msgpipe_msg)
	char buf[1024];
	int buflen;
} msgpipe;

extern int msgpipe_open(msgpipe* pipe, const char *name, bool reverse);
extern void msgpipe_close(msgpipe* pipe);
extern int msgpipe_send(msgpipe* pipe, msgpipe_msg* msg);
extern int msgpipe_poll(msgpipe* pipe, msgpipe_msg* msg);

#endif