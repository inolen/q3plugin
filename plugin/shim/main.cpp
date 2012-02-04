#include <limits.h>
#include <dlfcn.h>
#include <SDL.h>
#include <unistd.h>
#include "../lib/msgpipe.h"

#define FIFO_NAME "q3plugin"

extern "C" void Cmd_ExecuteString(const char*);
int (*OG_SDL_PushEvent)(SDL_Event*) = NULL;
static void process_messages();

msgpipe::fdxpipe g_msgpipe;

/**
 * Hook the functions provided by libsdl.
 */
extern "C" SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode) {
	static SDL_GrabMode state = SDL_GRAB_OFF;

	if (mode != SDL_GRAB_QUERY) {
		state = mode;

		msgpipe::message msg;
		msg.type = msgpipe::msgs::MOUSELOCK;
		msg.mouselock.lock = state == SDL_GRAB_ON;
		g_msgpipe.send(msg);
	}

	return state;
}

extern "C" int SDL_PollEvent(SDL_Event* event) {
	static int (*func)(SDL_Event*) = NULL;

	if (!func) {
		func = (int (*)(SDL_Event*))dlsym(RTLD_NEXT, "SDL_PollEvent");
	}

	// Poll our own event queue when SDL polls.
	process_messages();

	// Call the original SD_PollEvent.
	return (*func)(event);
}

/**
 * Shim specific functions.
 */
static char *curprocpath() {
	static char path[PATH_MAX];

	int len = readlink("/proc/self/exe", path, PATH_MAX - 1);

	if (len == -1) {
		return NULL;
	}

	return path;
}

static void process_messages() {
	msgpipe::message msg;

	while (g_msgpipe.poll(msg)) {
		switch (msg.type) {
			case msgpipe::msgs::SDLEVENT:
				OG_SDL_PushEvent(&msg.sdlevent.event);
				break;

			case msgpipe::msgs::GAMECMD:
				Cmd_ExecuteString(msg.gamecmd.text);
				break;
		}
	}
}

static int should_enable() {
	char* current_exe = NULL;
	char* base_name = NULL;

	if ((current_exe = curprocpath()) == NULL) {
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
	if (!should_enable()) {
		return;
	}

	// Get the address of the original SDL_PushEvent function.
	OG_SDL_PushEvent = (int (*)(SDL_Event*))dlsym(RTLD_NEXT, "SDL_PushEvent");

	// Initialize message pipe.
	if (!g_msgpipe.open(std::string(FIFO_NAME), true)) {
		return;
	}
}

__attribute__((destructor)) static void unload(void) {
	g_msgpipe.close();
}