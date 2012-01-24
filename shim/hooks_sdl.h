#ifndef HOOKS_SDL_H
#define HOOKS_SDL_H

#include <SDL.h>

extern int (*OG_SDL_PushEvent)(SDL_Event*);

SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode);
int SDL_PollEvent(SDL_Event* event);

#endif