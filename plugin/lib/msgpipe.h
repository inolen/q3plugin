#ifndef MSGPIPE_H
#define MSGPIPE_H

#include <string>
#include <SDL.h>

namespace msgpipe {

// Message types.
namespace msgs {
typedef enum {
	SDLEVENT,
	GAMECMD,
	MOUSELOCK
} types;
}

// Messages sent from the plugin -> shim.
typedef struct sdlevent_s {
	int       type;
	SDL_Event event;
} sdlevent_t;

typedef struct gamecmd_s {
	int  type;
	char text[128];
} gamecmd_t;

// Messages sent from the shim -> plugin.
typedef struct mouselock_s {
	int  type;
	bool lock;
} mouselock_t;

typedef union message {
	int         type;
	sdlevent_t  sdlevent;
	gamecmd_t   gamecmd;
	mouselock_t mouselock;
} message;

// Half-duplex pipe.
class hdxpipe {
public:
	bool open(const std::string& name, bool read);
	int  send(const message& msg);
	bool poll(message& msg);
	void close();

private:
	void pump();

	std::string name_;
	int  fd_;
	char buf_[1024]; // This needs to be larger than sizeof(MSGPipe_Message)
	int  buflen_;
};

// Full-duplex pipe.
class fdxpipe {
public:
	bool open(const std::string& name, bool reverse);
	int  send(const message& msg);
	bool poll(message& msg);
	void close();

private:
	hdxpipe piper_;
	hdxpipe pipew_;
};

}

#endif