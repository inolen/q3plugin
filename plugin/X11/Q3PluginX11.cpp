#include "Q3PluginX11.h"
//#define _GNU_SOURCE
#include <dlfcn.h>
#include <gdk/gdkx.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <libgen.h>
#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include "PluginWindowX11.h"
#include "../Q3PluginApi.h"

#define FIFO_NAME "q3plugin"

void Q3PluginX11::LaunchGame(FB::PluginWindow* window) {
	FB::PluginWindowX11 *xwindow = dynamic_cast<FB::PluginWindowX11*>(window);

	// SDL hijacking! Setting the SDL_WINDOWID environment variable will cause any
	// subsequent calls to SDL_SetVideoMade (like the one made in the ioquake3
	// initialization) use the specified window id for rendering, instead of
	// creating a new one.
	const int xid = xwindow->getWindow();
	char sxid[32];
	snprintf(sxid, 32, "%i", xid);
	setenv("SDL_WINDOWID", sxid, TRUE);

	// Install the shim. This shim gets loaded before any other shared assemblies,
	// allowing us to override functions in subsequently loaded assemblies. We use
	// this to do things such as intercepting the SDL event polling and
	// injecting our own translated events from GTK.
	char pluginDir[PATH_MAX];
	char shimPath[PATH_MAX];
	strncpy(pluginDir, getFSPath().c_str(), getFSPath().length());
	dirname(pluginDir);
	snprintf(shimPath, PATH_MAX, "%s/libq3plugshim.so", pluginDir);
	setenv("LD_PRELOAD", shimPath, TRUE);

	// Launch game.
	const char* argv[] = { "/home/inolen/quake3/ioquake3/build/release-linux-i386/ioquake3.i386", "+r_fullscreen", "0", NULL };
	pid_t pid;
	int status;
	g_spawn_async(NULL, (gchar**)argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, NULL);

	// The pipes are sycnchronous and will block until both ends are open. In order to avoid
	// deadlocking, we need to start the process in a new thread (which will open one side
	// of the pipe during the shim initialization) and then open our end of the pipe.
	if (msgpipe_open(&pipe_, FIFO_NAME, false) < 0) {
		fprintf(stderr, "Failed to make event pipe.\n");
		return;
	}

	// Wait for the process to end and then close it 100%.
	while (!waitpid(pid, &status, WNOHANG)) {
		msgpipe_msg msg;

		while (msgpipe_poll(&pipe_, &msg)) {
			ProcessMessage(&msg);
		}
	}

	g_spawn_close_pid(pid);

	// Close message pipe.
	msgpipe_close(&pipe_);
}

void Q3PluginX11::CenterMouse(FB::PluginWindow* window) {
	FB::PluginWindowX11 *xwindow = dynamic_cast<FB::PluginWindowX11*>(window);
	GtkWidget *widget = xwindow->getWidget();
	Display *xdsp = GDK_WINDOW_XDISPLAY(widget->window);
	Window xwnd = GDK_WINDOW_XWINDOW(widget->window);
	XWarpPointer(xdsp, None, xwnd, 0,0,0,0, window->getWindowWidth()/2, window->getWindowHeight()/2);
}
