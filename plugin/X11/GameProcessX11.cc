#include "GameProcessX11.h"
#include <boost/filesystem/operations.hpp>
#include <glib.h>
#include <sys/wait.h>
#include "PluginWindow.h"
#include "PluginWindowX11.h"

namespace fs = boost::filesystem;

GameProcessX11::GameProcessX11(FB::PluginWindow *window, const std::string& path) :
	GameProcess(window, path) {
}

GameProcessX11::~GameProcessX11() {
}

bool GameProcessX11::SpawnNativeProcess() {
	FB::PluginWindowX11 *window = dynamic_cast<FB::PluginWindowX11*>(window_);

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
	fs::path p(path_);
	std::ostringstream shimPath;
	shimPath << p.parent_path() << "/libq3pluginshim.so";
	setenv("LD_PRELOAD", shimPath.str().c_str(), TRUE);

	// Launch game.
	g_spawn_async(NULL, (gchar**)argv_, NULL, (GSpawnFlags)0, NULL, NULL, &gamepid_, NULL);
}

bool GameProcessX11::KillNativeProcess() {
	// Request the child process terminate.
	kill(gamepid_, SIGTERM);

	// Wait for it to.
	waitpid(gamepid_, NULL, 0);
}