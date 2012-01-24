#include "Q3Plugin.h"
#include <SDL.h>
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>

#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include "PluginWindowX11.h"

#include "Q3PluginApi.h"

extern "C" {
#include "../lib/msgpipe.h"
}

#define FIFO_NAME "q3plugin"

msgpipe* g_pipe = NULL;

///////////////////////////////////////////////////////////////////////////////
/// @fn Q3Plugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void Q3Plugin::StaticInitialize() {
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Q3Plugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void Q3Plugin::StaticDeinitialize() {
}

Q3Plugin::Q3Plugin() {
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Q3Plugin destructor.
///////////////////////////////////////////////////////////////////////////////
Q3Plugin::~Q3Plugin() {
	// This is optional, but if you reset m_api (the shared_ptr to your JSAPI
	// root object) and tell the host to free the retained JSAPI objects then
	// unless you are holding another shared_ptr reference to your JSAPI object
	// they will be released here.
	releaseRootJSAPI();
	m_host->freeRetainedObjects();
}

void Q3Plugin::onPluginReady() {
	// When this is called, the BrowserHost is attached, the JSAPI object is
	// created, and we are ready to interact with the page and such.  The
	// PluginWindow may or may not have already fire the AttachedEvent at
	// this point.
}

void Q3Plugin::shutdown() {
	// This will be called when it is time for the plugin to shut down;
	// any threads or anything else that may hold a shared_ptr to this
	// object should be released here so that this object can be safely
	// destroyed. This is the last point that shared_from_this and weak_ptr
	// references to this object will be valid
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<Q3PluginApi> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr Q3Plugin::createJSAPI() {
	// m_host is the BrowserHost
	return boost::make_shared<Q3PluginApi>(FB::ptr_cast<Q3Plugin>(shared_from_this()), m_host);
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

	// Lock mouse position;
	FB::PluginWindowX11 *X11PluginWindow = dynamic_cast<FB::PluginWindowX11*>(window);
	GtkWidget *widget = X11PluginWindow->getWidget();
	Display* X11Display = GDK_WINDOW_XDISPLAY(widget->window);
	Window X11Window  = GDK_WINDOW_XWINDOW(widget->window);
	XWarpPointer(X11Display, None, X11Window, 0,0,0,0, 400, 300);
	return true;
}

bool Q3Plugin::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow* window) {
	boost::thread t(boost::bind(&Q3Plugin::BootstrapGame, this, window));
	return true;
}

bool Q3Plugin::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *) {
	return false;
}

void Q3Plugin::BootstrapGame(FB::PluginWindow* window) {
	FB::PluginWindowX11 *wnd = dynamic_cast<FB::PluginWindowX11*>(window);

	// SDL surface hijack.
	const int wid = wnd->getWindow();
	char swid[32];
	snprintf(swid, 32, "%i", wid);
	setenv("SDL_WINDOWID", swid, TRUE);

	// Install shim.
	setenv("LD_PRELOAD", "/home/inolen/quake3/q3plugin/shim/libq3plugshim.so", TRUE);

	// Initialize message pipe.
	g_pipe = msgpipe_open(FIFO_NAME/*, PIPE_WRITE*/);

	if (!g_pipe) {
		fprintf(stderr, "Failed to make event pipe.\n");
		return;
	}

	// Launch game.
	const char* argv[] = { "/home/inolen/quake3/ioquake3/build/release-linux-i386/ioquake3.i386", NULL };
	g_spawn_sync(NULL, (gchar**)argv, NULL, (GSpawnFlags)0, NULL, NULL, NULL, NULL, NULL, NULL);

	// Close message pipe.
	msgpipe_close(g_pipe);
}