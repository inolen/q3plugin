#define _GNU_SOURCE
#include "hooks_sdl.h"
#include <dlfcn.h>
#include <stdio.h>
#include "main.h"

#define FN(ptr,type,name,args) ptr = (type (*)args)dlsym (RTLD_NEXT, name)

int (*OG_SDL_PushEvent)(SDL_Event*) = NULL;

SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode) {
	//static int (*func)(SDL_GrabMode);
	//FN(func, SDL_GrabMode, "SDL_WM_GrabInput", (SDL_GrabMode));

	/*if (mode != SDL_GrabMode) {
		mouseLocked = mose == SDL_GRAB_ON;
	}

	return mouseLocked ? SDL_GRAB_ON : SDL_GRAB_OFF;*/
	return SDL_GRAB_OFF;
}

int SDL_PollEvent(SDL_Event* event) {
	// Store pointer to original SD_PollEvent.
	static int (*func)(SDL_Event*);
	FN(func, int, "SDL_PollEvent", (SDL_Event*));

	// Poll our own event queue when SDL polls.
	msgpipe_msg msg;

	while (msgpipe_poll(g_pipe, &msg)) {
		process_message(&msg);
	}

	// Call the original SD_PollEvent.
	return (*func)(event);
}

void hooks_sdl_init() {
	// Store a reference to the real SDL_PushEvent.
	FN(OG_SDL_PushEvent, int, "SDL_PushEvent", (SDL_Event*));
}
