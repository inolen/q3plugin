#include "main.h"
#include <SDL.h>
#include <unistd.h>
#include "hooks_sdl.h"
#include "../lib/msgpipe.h"

#define FIFO_NAME "q3plugin"

void __attribute__ ((constructor)) load(void);
void __attribute__ ((destructor)) unload(void);

msgpipe* g_pipe = NULL;

void process_message(msgpipe_msg* msg) {
	SDL_Event event;

	switch (msg->type) {
		case KEYPRESS: {
			msg_keypress *kp = (msg_keypress*)msg;
			event.type = kp->pressing ? SDL_KEYDOWN : SDL_KEYUP;
			event.key.keysym.sym = kp->key;
			event.key.keysym.unicode = kp->key;
			OG_SDL_PushEvent(&event);
		}
		break;

		case MOUSEPRESS: {
			msg_mousepress *mp = (msg_mousepress*)msg;
			event.type = mp->pressing ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;

			switch (mp->button) {
				case MOUSE_LEFT:
					event.button.button = 1;
					break;

				case MOUSE_MIDDLE:
					event.button.button = 2;
					break;

				case MOUSE_RIGHT:
					event.button.button = 3;
					break;
			}

			OG_SDL_PushEvent(&event);
		}
		break;

		case MOUSEMOTION: {
			msg_mousemotion *mm = (msg_mousemotion*)msg;
			event.type = SDL_MOUSEMOTION;
			event.motion.xrel = mm->xrel;
			event.motion.yrel = mm->yrel;
			OG_SDL_PushEvent(&event);
		}
		break;
	}
}

int should_shim() {
	char current_exe[512];
	char *base_name;

	int len = readlink("/proc/self/exe", current_exe, 512 - 1);

	if (len == -1) {
		return 0;
	}

	base_name = basename(current_exe);

	return strstr(base_name, "quake3") != NULL;
}

void load(void) {
	// Major hack to avoid conflicts with other processes q3 launches.
	if (!should_shim()) {
		return;
	}

	// Initialize SDL hooks.
	hooks_sdl_init();

	// Initialize message pipe.
	g_pipe = msgpipe_open(FIFO_NAME, PIPE_READ);

	if (!g_pipe) {
		fprintf(stderr, "Failed to make event pipe.\n");
		return;
	}
}

void unload(void) {
	msgpipe_close(g_pipe);
}