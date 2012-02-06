#include <limits.h>
#include <dlfcn.h>
#include <SDL.h>
#include <unistd.h>
#include "../MessagePipe.h"

extern "C" void Cmd_ExecuteString(const char*);
int (*OG_SDL_PushEvent)(SDL_Event*) = NULL;

static void SendMessage(message_t& msg);
static void ProcessMessages();

MessagePipe *g_msgpipe = NULL;

/**
 * Hook the functions provided by libsdl.
 */
extern "C" SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode) {
	static SDL_GrabMode state = SDL_GRAB_OFF;

	if (mode != SDL_GRAB_QUERY) {
		state = mode;

		message_t msg;
		msg.type = MOUSELOCK;
		msg.mouselock.lock = state == SDL_GRAB_ON;

		SendMessage(msg);
	}

	return state;
}

extern "C" int SDL_PollEvent(SDL_Event *event) {
	static int (*func)(SDL_Event*) = NULL;

	if (!func) {
		func = (int (*)(SDL_Event*))dlsym(RTLD_NEXT, "SDL_PollEvent");
	}

	// Poll our own event queue when SDL polls.
	ProcessMessages();

	// Call the original SD_PollEvent.
	return (*func)(event);
}

/**
 * Shim specific functions.
 */
static char *CurrentProcessPath() {
	static char path[PATH_MAX];

	int len = readlink("/proc/self/exe", path, PATH_MAX - 1);

	if (len == -1) {
		return NULL;
	}

	return path;
}

static void SendMessage(message_t& msg) {
	g_msgpipe->Send(msg);
}

static void ProcessMessages() {
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

static int ShouldEnable() {
	char *current_exe = NULL;
	char *base_name = NULL;

	if ((current_exe = CurrentProcessPath()) == NULL) {
		return 0;
	}

	base_name = basename(current_exe);

	return strstr(base_name, "quake3") != NULL;
}

/**
 * Called when the shared library is loaded/unloaded by a process.
 */
__attribute__((constructor)) static void load(void) {
	// Major hack to avoid conflicts with other processes q3 launches.
	if (!ShouldEnable()) {
		return;
	}

	// Get the address of the original SDL_PushEvent function.
	OG_SDL_PushEvent = (int (*)(SDL_Event*))dlsym(RTLD_NEXT, "SDL_PushEvent");

	// Initialize message pipe.
	g_msgpipe = new MessagePipe("message_queue", false);
}

__attribute__((destructor)) static void unload(void) {
	delete g_msgpipe;
}