#include "Q3Plugin.h"
#include <boost/filesystem/operations.hpp>
#include <unistd.h>
#include "FactoryBase.h"
#include "Q3PluginApi.h"

// TODO Merge this with Q3PluginFactory.
#if defined FB_WIN
#include "X11/GameProcessWin.h"
#define Q3PROCESS GameProcessWin
#elif defined FB_MACOSX
#include "X11/GameProcessMac.h"
#define Q3PROCESS GameProcessMac
#else
#include "X11/GameProcessX11.h"
#define Q3PROCESS GameProcessX11
#endif

namespace fs = boost::filesystem;

void Q3Plugin::StaticInitialize() {
}

void Q3Plugin::StaticDeinitialize() {
}

Q3Plugin::Q3Plugin() : message_pipe_(NULL), current_process_(NULL) {
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

/**
 * Send/receive custom messages between this plugin and the shim.
 */
void Q3Plugin::SendMessage(message_t& msg) {
	message_pipe_->Send(msg);
}

void Q3Plugin::ProcessMessage(message_t& msg) {
}

void Q3Plugin::PumpMessages() {
	message_t msg;

	while (message_pipe_->Poll(msg)) {
		ProcessMessage(msg);
	}
}

bool Q3Plugin::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *window) {
	// Create the message event pumping thread.
	message_pipe_ = new MessagePipe("message_queue", true);
	message_timer_ = FB::Timer::getTimer(1000, true, boost::bind(&Q3Plugin::PumpMessages, this));
	message_timer_->start();

	int width = window->getWindowWidth();
	int height = window->getWindowHeight();

	// Load ioquake3 from the same directory as the plugin.
	fs::path p(getFSPath());
	std::ostringstream gamePath;
	gamePath << p.parent_path() << "/ioquake3";

	// Create a game instance.
	current_process_ = new Q3PROCESS(window, gamePath.str());
	current_process_->Launch(width, height);

	return true;
}

bool Q3Plugin::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *window) {
	if (current_process_ != NULL) {
		delete current_process_;
	}

	message_timer_->stop();

	return false;
}