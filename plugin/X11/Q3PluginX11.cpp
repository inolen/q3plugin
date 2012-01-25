#include "Q3PluginX11.h"
#include <glib.h>
#include <unistd.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <sys/wait.h>
#include "PluginWindowX11.h"

#define FIFO_NAME "q3plugin"

void Q3PluginX11::LaunchGame(FB::PluginWindow* window) {
	FB::PluginWindowX11 *xwindow = dynamic_cast<FB::PluginWindowX11*>(window);
	GPid pid;
	int status;
	const char* argv[] = { "/home/inolen/quake3/ioquake3/build/release-linux-i386/ioquake3.i386", NULL };

	// SDL surface hijack.
	const int xid = xwindow->getWindow();
	char sxid[32];
	snprintf(sxid, 32, "%i", xid);
	setenv("SDL_WINDOWID", sxid, TRUE);

	// Install shim.
	setenv("LD_PRELOAD", "/home/inolen/quake3/q3plugin/shim/libq3plugshim.so", TRUE);

	// Launch game.
	g_spawn_async(NULL, (gchar**)argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, NULL);

	// The pipes are sycnchronous and will block until both ends are open. Open the pipe after
	// we spawn the process so we don't deadlock.
	pipe_ = msgpipe_open(FIFO_NAME, PIPE_WRITE);

	if (!pipe_) {
		fprintf(stderr, "Failed to make event pipe.\n");
		return;
	}

	waitpid(pid, &status, 0);
	g_spawn_close_pid(pid);

	// Close message pipe.
	msgpipe_close(pipe_);
}

void Q3PluginX11::CenterMouse(FB::PluginWindow* window) {
	FB::PluginWindowX11 *xwindow = dynamic_cast<FB::PluginWindowX11*>(window);
	GtkWidget *widget = xwindow->getWidget();
	Display *xdsp = GDK_WINDOW_XDISPLAY(widget->window);
	Window xwnd = GDK_WINDOW_XWINDOW(widget->window);
	XWarpPointer(xdsp, None, xwnd, 0,0,0,0, window->getWindowWidth()/2, window->getWindowHeight()/2);
}
