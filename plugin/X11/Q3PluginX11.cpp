#include "Q3PluginX11.h"
#include <boost/filesystem/operations.hpp>
#include <glib.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <libgen.h>
#include <SDL.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "PluginEvents/KeyCodes.h"
#include "PluginWindowX11.h"
#include "../Q3PluginApi.h"

namespace fs = boost::filesystem;

Q3PluginX11::Q3PluginX11() {
}

Q3PluginX11::~Q3PluginX11() {
}

void Q3PluginX11::LaunchGame(int argc, char** argv) {
	FB::PluginWindowX11 *window = dynamic_cast<FB::PluginWindowX11*>(GetWindow());

	// SDL hijacking! Setting the SDL_WINDOWID environment variable will cause any
	// subsequent calls to SDL_SetVideoMade (like the one made in the ioquake3
	// initialization) use the specified window id for rendering, instead of
	// creating a new one.
	const int xid = window->getWindow();
	std::ostringstream sxid;
	sxid << xid;
	setenv("SDL_WINDOWID", sxid.str().c_str(), TRUE);

	// Install the shim. This shim gets loaded before any other shared assemblies,
	// allowing us to override functions in subsequently loaded assemblies. We use
	// this to do things such as intercepting the SDL event polling and
	// injecting our own translated events from GTK.
	fs::path p(getFSPath());
	std::ostringstream shimPath;
	shimPath << p.parent_path() << "/libq3plugshim.so";
	setenv("LD_PRELOAD", shimPath.str().c_str(), TRUE);

	// Launch game.
	g_spawn_async(NULL, (gchar**)argv, NULL, (GSpawnFlags)0, NULL, NULL, &gamepid_, NULL);
}

void Q3PluginX11::ShutdownGame() {
	// Request the child process terminate.
	kill(gamepid_, SIGTERM);

	// Wait for it to.
	waitpid(gamepid_, NULL, 0);
}

void Q3PluginX11::ProcessMessage(msgpipe::message& msg) {
	Q3Plugin::ProcessMessage(msg);

	if (msg.type == msgpipe::msgs::MOUSELOCK) {
		mouseLocked_ = msg.mouselock.lock;
	}
}

int FBtoSDLkey(FB::FBKeyCode key) {
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

bool Q3PluginX11::onKeyDown(FB::KeyDownEvent* evt, FB::PluginWindow* window) {
	int key = evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255 ? evt->m_os_key_code : FBtoSDLkey(evt->m_key_code);

	/*std::ostringstream test;
	test << "Translated: " << key;
	m_host->htmlLog(test.str());*/

	if (key != -1) {
		msgpipe::message msg;
		msg.type = msgpipe::msgs::SDLEVENT;
		msg.sdlevent.event.type = SDL_KEYDOWN;
		msg.sdlevent.event.key.keysym.sym = (SDLKey)key;
		msg.sdlevent.event.key.keysym.unicode = key;
		fdxpipe_.send(msg);
	}

	return true;
}

bool Q3PluginX11::onKeyUp(FB::KeyUpEvent* evt, FB::PluginWindow* window) {
	int key = evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255 ? evt->m_os_key_code : FBtoSDLkey(evt->m_key_code);

	if (key != -1) {
		msgpipe::message msg;
		msg.type = msgpipe::msgs::SDLEVENT;
		msg.sdlevent.event.type = SDL_KEYUP;
		msg.sdlevent.event.key.keysym.sym = (SDLKey)key;
		msg.sdlevent.event.key.keysym.unicode = key;
		fdxpipe_.send(msg);
	}

	return true;
}

bool Q3PluginX11::onMouseDown(FB::MouseDownEvent* evt, FB::PluginWindow* window) {
	msgpipe::message msg;
	msg.type = msgpipe::msgs::SDLEVENT;
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

	fdxpipe_.send(msg);

	return true;
}

bool Q3PluginX11::onMouseUp(FB::MouseUpEvent* evt, FB::PluginWindow* window) {
	msgpipe::message msg;
	msg.type = msgpipe::msgs::SDLEVENT;
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

	fdxpipe_.send(msg);

	return true;
}

bool Q3PluginX11::onMouseMove(FB::MouseMoveEvent* evt, FB::PluginWindow* window) {
	if (mouseLocked_) {
		// Forward the event.
		msgpipe::message msg;
		msg.type = msgpipe::msgs::SDLEVENT;
		msg.sdlevent.event.type = SDL_MOUSEMOTION;
		msg.sdlevent.event.motion.xrel = evt->m_x - window->getWindowWidth()/2;
		msg.sdlevent.event.motion.yrel = evt->m_y - window->getWindowHeight()/2;
		fdxpipe_.send(msg);

		// Center the mouse.
		FB::PluginWindowX11 *window = dynamic_cast<FB::PluginWindowX11*>(GetWindow());
		GtkWidget *widget = window->getWidget();
		Display *xdsp = GDK_WINDOW_XDISPLAY(widget->window);
		Window xwnd = GDK_WINDOW_XWINDOW(widget->window);
		XWarpPointer(xdsp, None, xwnd, 0,0,0,0, window->getWindowWidth()/2, window->getWindowHeight()/2);
	}

	return true;
}