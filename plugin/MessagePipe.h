#ifndef MESSAGEPIPE_H
#define MESSAGEPIPE_H

#include <string>
#include <SDL.h>

// Forward declare.
namespace boost { namespace interprocess {
class message_queue;
}}

typedef enum {
	SDLEVENT,
	GAMECMD,
	MOUSELOCK
} types;

// Messages sent from the plugin -> shim.
typedef struct {
	SDL_Event event;
} sdlevent_t;

typedef struct {
	char text[128];
} gamecmd_t;

// Messages sent from the shim -> plugin.
typedef struct {
	bool lock;
} mouselock_t;

typedef struct {
	int type;
	int num;

	union {
		sdlevent_t sdlevent;
		gamecmd_t gamecmd;
		mouselock_t mouselock;
	};
} message_t;

class MessagePipe {
public:
	MessagePipe(const std::string& name, bool create);
	~MessagePipe();

	bool Send(message_t& msg);
	bool Poll(message_t& msg);

private:
	boost::interprocess::message_queue *read_queue_;
	boost::interprocess::message_queue *write_queue_;
};

#endif