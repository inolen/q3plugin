#ifndef Q3PLUGIN_H
#define Q3PLUGIN_H

#include "PluginWindow.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginCore.h"

extern "C" {
#include "lib/msgpipe.h"
}

FB_FORWARD_PTR(Q3Plugin)
class Q3Plugin : public FB::PluginCore {
public:
	static void StaticInitialize();
	static void StaticDeinitialize();

	Q3Plugin();
	virtual ~Q3Plugin();

protected:
	virtual FB::JSAPIPtr createJSAPI();
	// If you want your plugin to always be windowless, set this to true
	// If you want your plugin to be optionally windowless based on the
	// value of the "windowless" param tag, remove this method or return
	// FB::PluginCore::isWindowless()
	virtual bool isWindowless() { return false; }

	void ProcessMessage(msgpipe_msg *msg);
	virtual void LaunchGame(FB::PluginWindow* window) = 0;
	virtual void CenterMouse(FB::PluginWindow* window) = 0;

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
	virtual bool onKeyDown(FB::KeyDownEvent* evt, FB::PluginWindow* window);
	virtual bool onKeyUp(FB::KeyUpEvent* evt, FB::PluginWindow* window);
	virtual bool onMouseDown(FB::MouseDownEvent* evt, FB::PluginWindow* window);
	virtual bool onMouseUp(FB::MouseUpEvent* evt, FB::PluginWindow* window);
	virtual bool onMouseMove(FB::MouseMoveEvent* evt, FB::PluginWindow* window);
	virtual bool onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow* window);
	virtual bool onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow* window);
	/** END EVENTDEF -- DON'T CHANGE THIS LINE **/

	msgpipe pipe_;
	bool lockmouse_;
};

#endif
