#include "Q3Plugin.h"
#include <SDL.h>
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>
#include "Q3PluginApi.h"

#ifdef FB_X11
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <sys/wait.h>
#include "PluginWindowX11.h"
#endif

extern "C" {
#include "../lib/msgpipe.h"
}

#define FIFO_NAME "q3plugin"

msgpipe* g_pipe = NULL;

void Q3Plugin::StaticInitialize() {
}

void Q3Plugin::StaticDeinitialize() {
}

Q3Plugin::Q3Plugin() {
}

Q3Plugin::~Q3Plugin() {
	// This is optional, but if you reset m_api (the shared_ptr to your JSAPI
	// root object) and tell the host to free the retained JSAPI objects then
	// unless you are holding another shared_ptr reference to your JSAPI object
	// they will be released here.
	releaseRootJSAPI();
	m_host->freeRetainedObjects();
}

FB::JSAPIPtr Q3Plugin::createJSAPI() {
	// m_host is the BrowserHost
	return boost::make_shared<Q3PluginApi>(FB::ptr_cast<Q3Plugin>(shared_from_this()), m_host);
}

void Q3Plugin::BootstrapGame(FB::PluginWindow* window) {
	FB::PluginWindowX11 *wnd = dynamic_cast<FB::PluginWindowX11*>(window);
	GPid pid;
	int status;
	const char* argv[] = { "/home/inolen/quake3/ioquake3/build/release-linux-i386/ioquake3.i386", NULL };

	// SDL surface hijack.
	const int wid = wnd->getWindow();
	char swid[32];
	snprintf(swid, 32, "%i", wid);
	setenv("SDL_WINDOWID", swid, TRUE);

#ifdef FB_X11
	// Install shim.
	setenv("LD_PRELOAD", "/home/inolen/quake3/q3plugin/shim/libq3plugshim.so", TRUE);
#endif

	// Launch game.
	g_spawn_async(NULL, (gchar**)argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, NULL);

#ifdef FB_X11
	// The pipes are sycnchronous and will block until both ends are open. Open the pipe after
	// we spawn the process so we don't deadlock.
	g_pipe = msgpipe_open(FIFO_NAME, PIPE_WRITE);

	if (!g_pipe) {
		fprintf(stderr, "Failed to make event pipe.\n");
		return;
	}
#endif

	waitpid(pid, &status, 0);
	g_spawn_close_pid(pid);

#ifdef FB_X11
	// Close message pipe.
	msgpipe_close(g_pipe);
#endif
}

bool Q3Plugin::onKeyDown(FB::KeyDownEvent* evt, FB::PluginWindow* window) {
	if (evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255) {
		msgpipe_msg msg;
		msg.type = KEYPRESS;
		msg.keypress.pressing = true;
		msg.keypress.key = evt->m_os_key_code;
		msgpipe_send(g_pipe, &msg);
	}

	return true;
}

bool Q3Plugin::onKeyUp(FB::KeyUpEvent* evt, FB::PluginWindow* window) {
	if (evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255) {
		msgpipe_msg msg;
		msg.type = KEYPRESS;
		msg.keypress.pressing = false;
		msg.keypress.key = evt->m_os_key_code;
		msgpipe_send(g_pipe, &msg);
	}

	return true;
}

bool Q3Plugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *) {
	msgpipe_msg msg;
	msg.type = MOUSEPRESS;
	msg.mousepress.pressing = true;

	switch (evt->m_Btn) {
		case 0:
			msg.mousepress.button = MOUSE_LEFT;
			break;
		case 1:
			msg.mousepress.button = MOUSE_MIDDLE;
			break;
		case 2:
			msg.mousepress.button = MOUSE_RIGHT;
			break;
	}

	msgpipe_send(g_pipe, &msg);

	return true;
}

bool Q3Plugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *) {
	msgpipe_msg msg;
	msg.type = MOUSEPRESS;
	msg.mousepress.pressing = false;

	switch (evt->m_Btn) {
		case 0:
			msg.mousepress.button = MOUSE_LEFT;
			break;
		case 1:
			msg.mousepress.button = MOUSE_MIDDLE;
			break;
		case 2:
			msg.mousepress.button = MOUSE_RIGHT;
			break;
	}

	msgpipe_send(g_pipe, &msg);

	return true;
}

bool Q3Plugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *window) {
	msgpipe_msg msg;
	msg.type = MOUSEMOTION;
	msg.mousemotion.xrel = evt->m_x - 400;
	msg.mousemotion.yrel = evt->m_y - 300;
	msgpipe_send(g_pipe, &msg);

#ifdef FB_X11
	// Lock mouse position;
	FB::PluginWindowX11 *X11PluginWindow = dynamic_cast<FB::PluginWindowX11*>(window);
	GtkWidget *widget = X11PluginWindow->getWidget();
	Display* X11Display = GDK_WINDOW_XDISPLAY(widget->window);
	Window X11Window  = GDK_WINDOW_XWINDOW(widget->window);
	XWarpPointer(X11Display, None, X11Window, 0,0,0,0, 400, 300);
#endif

	return true;
}

bool Q3Plugin::onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow* window) {
	boost::thread t(boost::bind(&Q3Plugin::BootstrapGame, this, window));
	return true;
}

bool Q3Plugin::onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow* window) {
	return false;
}