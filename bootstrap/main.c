#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "msgpipe.h"

#define FIFO_NAME "q3pluggedin"

msgpipe* g_pipe;

void bootstrap_game(void* param) {
	GtkWidget* window = (GtkWidget*)param;

	// SDL surface hijack.
	const int wid = GDK_WINDOW_XWINDOW(window->window);
	char swid[32];
	snprintf(swid, 32, "%i", wid);
	setenv("SDL_WINDOWID", swid, TRUE);

	// Install shim.
	setenv("LD_PRELOAD", "/home/inolen/quake3/test/bootstrap/q3plugshim/libq3plugshim.so", TRUE);

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

gboolean gtk_event_callback(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	switch(event->type) {
		case GDK_KEY_PRESS:
		case GDK_KEY_RELEASE:
			{
				if (event->key.keyval >= 0 && event->key.keyval <= 255) {
					msgpipe_msg msg;
					msg.type = KEYPRESS;
					msg.keypress.pressing = event->type == GDK_KEY_PRESS;
					msg.keypress.key = event->key.keyval;
					msgpipe_send(g_pipe, &msg);
				}
			}
			break;

		case GDK_BUTTON_PRESS:
		case GDK_BUTTON_RELEASE:
			{
				msgpipe_msg msg;
				msg.type = MOUSEPRESS;
				msg.mousepress.pressing = event->type == GDK_BUTTON_PRESS;
				msg.mousepress.button = event->button.button;
				msgpipe_send(g_pipe, &msg);
			}
			break;

		case GDK_MOTION_NOTIFY:
			{
				msgpipe_msg msg;
				msg.type = MOUSEMOTION;
				msg.mousemotion.xrel = event->motion.x - 400;
				msg.mousemotion.yrel = event->motion.y - 300;
				msgpipe_send(g_pipe, &msg);
			}
			break;

		default:
			break;
	}

	// Lock mouse position;
	Display* display = GDK_WINDOW_XDISPLAY(widget->window);
	Window window  = GDK_WINDOW_XWINDOW(widget->window);
	XWarpPointer(display, None, window, 0,0,0,0, 400, 300);

	return FALSE;
}

int main(int argc, char* argv[]) {
	GtkWidget *window;

	gtk_init(&argc, &argv);

	// Create window.
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size((GtkWindow*)window, 800, 600);
	gtk_widget_show(window);

	// Attach event handlers.
	gtk_widget_add_events(
		window,
		GDK_BUTTON_PRESS_MASK |
		GDK_BUTTON_RELEASE_MASK |
		GDK_KEY_PRESS_MASK |
		GDK_KEY_RELEASE_MASK |
		GDK_POINTER_MOTION_MASK |
		GDK_POINTER_MOTION_HINT_MASK
	);
	g_signal_connect(G_OBJECT(window), "event", G_CALLBACK(gtk_event_callback), NULL);

	// Run the pump so the window is valid before we bootstrap.
	while (gtk_events_pending()) {
		gtk_main_iteration_do(FALSE);
	}

	// Startup the game.
	g_thread_create((GThreadFunc)&bootstrap_game, window, FALSE, NULL);

	// Enter main GTK event loop.
	gtk_main();

	return 0;
}