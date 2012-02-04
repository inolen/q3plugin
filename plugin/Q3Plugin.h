#ifndef Q3PLUGIN_H
#define Q3PLUGIN_H

#include <boost/thread.hpp>
#include "PluginWindow.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginCore.h"
#include "lib/msgpipe.h"

FB_FORWARD_PTR(Q3Plugin)
class Q3Plugin : public FB::PluginCore {
public:
	static void StaticInitialize();
	static void StaticDeinitialize();

	Q3Plugin();
	virtual ~Q3Plugin();

	void Connect(std::string server);

protected:
	virtual void ProcessMessage(msgpipe::message& msg);
	virtual void LaunchGame(int argc, char** argv) = 0;
	virtual void ShutdownGame() = 0;

	// If you want your plugin to always be windowless, set this to true
	// If you want your plugin to be optionally windowless based on the
	// value of the "windowless" param tag, remove this method or return
	// FB::PluginCore::isWindowless()
	virtual bool isWindowless() { return false; }
	virtual FB::JSAPIPtr createJSAPI();

	BEGIN_PLUGIN_EVENT_MAP()
		EVENTTYPE_CASE(FB::KeyDownEvent, onKeyDown, FB::PluginWindow)
		EVENTTYPE_CASE(FB::KeyUpEvent, onKeyUp, FB::PluginWindow)
		EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
		EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
		EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
		EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
		EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
	END_PLUGIN_EVENT_MAP()

	/** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
	virtual bool onKeyDown(FB::KeyDownEvent* evt, FB::PluginWindow* window) = 0;
	virtual bool onKeyUp(FB::KeyUpEvent* evt, FB::PluginWindow* window) = 0;
	virtual bool onMouseDown(FB::MouseDownEvent* evt, FB::PluginWindow* window) = 0;
	virtual bool onMouseUp(FB::MouseUpEvent* evt, FB::PluginWindow* window) = 0;
	virtual bool onMouseMove(FB::MouseMoveEvent* evt, FB::PluginWindow* window) = 0;
	virtual bool onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow* window);
	virtual bool onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow* window);
	/** END EVENTDEF -- DON'T CHANGE THIS LINE **/

	msgpipe::fdxpipe msgpipe_;

private:
	void RunMessagePump();
	void StartMessagePump();
	void StopMessagePump();

	boost::thread pumpThread_;
	char** gameArgs_;
};

#endif
