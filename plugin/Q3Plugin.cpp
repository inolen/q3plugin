#include "Q3Plugin.h"
#include <assert.h>
#include <boost/filesystem/operations.hpp>
#include <SDL.h>
#include <unistd.h>
#include "Q3PluginApi.h"

#define FIFO_NAME "q3plugin"

namespace fs = boost::filesystem;

void Q3Plugin::StaticInitialize() {
}

void Q3Plugin::StaticDeinitialize() {
}

Q3Plugin::Q3Plugin() : gameArgs_(NULL) {
}

Q3Plugin::~Q3Plugin() {
	//boost::interprocess::named_mutex::remove("q3plugin");

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

void Q3Plugin::Connect(std::string server) {
	msgpipe::message msg;
	msg.type = msgpipe::msgs::GAMECMD;
	strncpy(msg.gamecmd.text, std::string("connect ").append(server).c_str(), sizeof(msg.gamecmd.text));
	fdxpipe_.send(msg);
}

void Q3Plugin::ProcessMessage(msgpipe::message& msg) {
}

void Q3Plugin::RunMessagePump() {
	try {
		if (!fdxpipe_.open(std::string(FIFO_NAME), false)) {
			fprintf(stderr, "Failed to make event pipe.\n");
			return;
		}

		while (true) {
			msgpipe::message msg;

			while (fdxpipe_.poll(msg)) {
				ProcessMessage(msg);
			}

			boost::this_thread::sleep(boost::posix_time::milliseconds(1000 / 60));
		}
	}
	catch (boost::thread_interrupted const&) {
	}
}

void Q3Plugin::StartMessagePump() {
	pumpThread_ = boost::thread(&Q3Plugin::RunMessagePump, this);
}

void Q3Plugin::StopMessagePump() {
	pumpThread_.interrupt();

	// Close message pipe.
	fdxpipe_.close();

	pumpThread_.join();
}

bool Q3Plugin::onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow* window) {
	// Load ioquake3 from the same directory as the plugin.
	fs::path p(getFSPath());
	std::ostringstream gamePath;
	gamePath << p.parent_path() << "/ioquake3";

	gameArgs_ = (char**)malloc(sizeof(char*) * 10);
	memset(gameArgs_, 0, sizeof(char*) * 10);

	gameArgs_[0] = strdup(gamePath.str().c_str());
	gameArgs_[1] = strdup("+r_fullscreen");
	gameArgs_[2] = strdup("0");
	gameArgs_[3] = strdup("+r_mode");
	gameArgs_[4] = strdup("-1");
	gameArgs_[5] = strdup("+r_customWidth");
	gameArgs_[6] = (char*)malloc(sizeof(char) * 8);
	snprintf(gameArgs_[6], 8, "%i", window->getWindowWidth());
	gameArgs_[7] = strdup("+r_customHeight");
	gameArgs_[8] = (char*)malloc(sizeof(char) * 8);
	snprintf(gameArgs_[8], 8, "%i", window->getWindowHeight());

	// Run the message pump.
	StartMessagePump();

	LaunchGame(9, gameArgs_);

	return true;
}

bool Q3Plugin::onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow* window) {
	ShutdownGame();

	// Kill the message pump.
	StopMessagePump();

	// Free up arguments.
	if (gameArgs_ != NULL) {
		char** p = gameArgs_;

		while (*p != NULL) {
			free(*p);
			p++;
		}

		free(gameArgs_);
	}

	return false;
}