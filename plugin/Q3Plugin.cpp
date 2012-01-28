#include "Q3Plugin.h"
#include <SDL.h>
#include <unistd.h>
#include "Q3PluginApi.h"

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

void Q3Plugin::ProcessMessage(msgpipe_msg *msg) {
	if (msg->type == MOUSELOCK) {
		lockmouse_ = msg->mouselock.lock;
	}
}

bool Q3Plugin::onKeyDown(FB::KeyDownEvent* evt, FB::PluginWindow* window) {
	if (evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255) {
		msgpipe_msg msg;
		msg.type = KEYPRESS;
		msg.keypress.pressing = true;
		msg.keypress.key = evt->m_os_key_code;
		msgpipe_send(&pipe_, &msg);
	}

	return true;
}

bool Q3Plugin::onKeyUp(FB::KeyUpEvent* evt, FB::PluginWindow* window) {
	if (evt->m_os_key_code >= 0 && evt->m_os_key_code <= 255) {
		msgpipe_msg msg;
		msg.type = KEYPRESS;
		msg.keypress.pressing = false;
		msg.keypress.key = evt->m_os_key_code;
		msgpipe_send(&pipe_, &msg);
	}

	return true;
}

bool Q3Plugin::onMouseDown(FB::MouseDownEvent* evt, FB::PluginWindow* window) {
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

	msgpipe_send(&pipe_, &msg);

	return true;
}

bool Q3Plugin::onMouseUp(FB::MouseUpEvent* evt, FB::PluginWindow* window) {
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

	msgpipe_send(&pipe_, &msg);

	return true;
}

bool Q3Plugin::onMouseMove(FB::MouseMoveEvent* evt, FB::PluginWindow* window) {
	if (lockmouse_) {
		msgpipe_msg msg;
		msg.type = MOUSEMOTION;
		msg.mousemotion.xrel = evt->m_x - window->getWindowWidth()/2;
		msg.mousemotion.yrel = evt->m_y - window->getWindowHeight()/2;
		msgpipe_send(&pipe_, &msg);

		CenterMouse(window);
	}

	return true;
}

bool Q3Plugin::onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow* window) {
	boost::thread t(boost::bind(&Q3Plugin::LaunchGame, this, window));
	return true;
}

bool Q3Plugin::onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow* window) {
	return false;
}