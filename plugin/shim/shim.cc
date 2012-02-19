#include "shim.h"
#include <SDL.h>
#include "../MessagePipe.h"

MessagePipe *g_msgpipe = NULL;

extern "C" void Cmd_ExecuteString(const char*);
int (*OG_SDL_PushEvent)(SDL_Event*) = NULL;

static void Shim_SendMessage(message_t& msg);
static void Shim_ProcessMessages();

/**
 * SDL hooks.
 */
extern "C" SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode) {
	static SDL_GrabMode state = SDL_GRAB_OFF;

	if (mode != SDL_GRAB_QUERY) {
		state = mode;

		message_t msg;
		msg.type = MOUSELOCK;
		msg.mouselock.lock = state == SDL_GRAB_ON;

		Shim_SendMessage(msg);
	}

	return state;
}

extern "C" int SDL_PollEvent(SDL_Event *event) {
	static int (*func)(SDL_Event*) = NULL;

	if (!func) {
		func = (int (*)(SDL_Event*))GetFunctionAddress("SDL_PollEvent");
	}

	// Poll our own event queue when SDL polls.
	Shim_ProcessMessages();

	// Call the original SD_PollEvent.
	return (*func)(event);
}

/**
 * Core shim.
 */
static void Shim_SendMessage(message_t& msg) {
	g_msgpipe->Send(msg);
}

static void Shim_ProcessMessages() {
	message_t msg;

	while (g_msgpipe->Poll(msg)) {
		switch (msg.type) {
			case SDLEVENT:
				OG_SDL_PushEvent(&msg.sdlevent.event);
				break;

			case GAMECMD:
				Cmd_ExecuteString(msg.gamecmd.text);
				break;
		}
	}
}

static int Shim_ShouldEnable() {
	char *current_exe = NULL;
	char *base_name = NULL;

	if ((current_exe = GetCurrentProcessPath()) == NULL) {
		return 0;
	}

	base_name = basename(current_exe);

	return strstr(base_name, "quake3") != NULL;
}

void Shim_Initialize() {
	// Major hack to avoid conflicts with other processes q3 launches.
	if (!Shim_ShouldEnable()) {
		return;
	}

	// Get the address of the original SDL_PushEvent function.
	OG_SDL_PushEvent = (int (*)(SDL_Event*))GetFunctionAddress("SDL_PushEvent");

	// Initialize message pipe.
	g_msgpipe = new MessagePipe("message_queue", false);
}

void Shim_Shutdown() {
}