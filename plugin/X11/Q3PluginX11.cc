#include "Q3PluginX11.h"
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <libgen.h>
#include <SDL.h>
#include <signal.h>
#include <unistd.h>
#include "PluginEvents/KeyCodes.h"
#include "PluginWindowX11.h"
#include "../Q3PluginApi.h"

static int fb_to_sdl(FB::FBKeyCode key) {
	switch (key) {
		case FB::FBKEY_PAGEUP:
			return SDLK_PAGEUP;

		case FB::FBKEY_PAGEDOWN:
			return SDLK_PAGEDOWN;

		case FB::FBKEY_RETURN:
			return SDLK_RETURN;
	}

	return -1;
}

Q3PluginX11::Q3PluginX11() {
}

Q3PluginX11::~Q3PluginX11() {
}

void Q3PluginX11::ProcessMessage(message_t& msg) {
	Q3Plugin::ProcessMessage(msg);

	if (msg.type == MOUSELOCK) {
		mouseLocked_ = msg.mouselock.lock;
	}
}

bool Q3PluginX11::onKeyDown(FB::KeyDownEvent *evt, FB::PluginWindow *window) {
	int key = evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255 ? evt->m_os_key_code : fb_to_sdl(evt->m_key_code);

	if (key != -1) {
		message_t msg;
		msg.type = SDLEVENT;
		msg.sdlevent.event.type = SDL_KEYDOWN;
		msg.sdlevent.event.key.keysym.sym = (SDLKey)key;
		msg.sdlevent.event.key.keysym.unicode = key;
		SendMessage(msg);
	}

	return true;
}

bool Q3PluginX11::onKeyUp(FB::KeyUpEvent *evt, FB::PluginWindow *window) {
	int key = evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255 ? evt->m_os_key_code : fb_to_sdl(evt->m_key_code);

	if (key != -1) {
		message_t msg;
		msg.type = SDLEVENT;
		msg.sdlevent.event.type = SDL_KEYUP;
		msg.sdlevent.event.key.keysym.sym = (SDLKey)key;
		msg.sdlevent.event.key.keysym.unicode = key;
		SendMessage(msg);
	}

	return true;
}

bool Q3PluginX11::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *window) {
	message_t msg;
	msg.type = SDLEVENT;
	msg.sdlevent.event.type = SDL_MOUSEBUTTONDOWN;

	switch (evt->m_Btn) {
		case 0:
			msg.sdlevent.event.button.button = 1;
			break;

		case 1:
			msg.sdlevent.event.button.button = 2;
			break;

		case 2:
			msg.sdlevent.event.button.button = 3;
			break;
	}

	SendMessage(msg);

	return true;
}

bool Q3PluginX11::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *window) {
	message_t msg;
	msg.type = SDLEVENT;
	msg.sdlevent.event.type = SDL_MOUSEBUTTONUP;

	switch (evt->m_Btn) {
		case 0:
			msg.sdlevent.event.button.button = 1;
			break;

		case 1:
			msg.sdlevent.event.button.button = 2;
			break;

		case 2:
			msg.sdlevent.event.button.button = 3;
			break;
	}

	SendMessage(msg);

	return true;
}

bool Q3PluginX11::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *window) {
	if (mouseLocked_) {
		// Forward the event.
		message_t msg;
		msg.type = SDLEVENT;
		msg.sdlevent.event.type = SDL_MOUSEMOTION;
		msg.sdlevent.event.motion.xrel = evt->m_x - window->getWindowWidth()/2;
		msg.sdlevent.event.motion.yrel = evt->m_y - window->getWindowHeight()/2;
		SendMessage(msg);

		// Center the mouse.
		FB::PluginWindowX11 *window = dynamic_cast<FB::PluginWindowX11*>(GetWindow());
		GtkWidget *widget = window->getWidget();
		Display *xdsp = GDK_WINDOW_XDISPLAY(widget->window);
		Window xwnd = GDK_WINDOW_XWINDOW(widget->window);
		XWarpPointer(xdsp, None, xwnd, 0,0,0,0, window->getWindowWidth()/2, window->getWindowHeight()/2);
	}

	return true;
}