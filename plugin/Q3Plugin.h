#ifndef H_Q3Plugin
#define H_Q3Plugin

#include "PluginWindow.h"
#include "PluginEvents/AttachedEvent.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginCore.h"

static void Sys_Main(void*);
static void Sys_GLInit(FB::PluginWindow*);

FB_FORWARD_PTR(Q3Plugin)
class Q3Plugin : public FB::PluginCore
{
public:
	static void StaticInitialize();
	static void StaticDeinitialize();

	Q3Plugin();
	virtual ~Q3Plugin();

protected:
	void onPluginReady();
	void shutdown();

	virtual FB::JSAPIPtr createJSAPI();
	// If you want your plugin to always be windowless, set this to true
	// If you want your plugin to be optionally windowless based on the
	// value of the "windowless" param tag, remove this method or return
	// FB::PluginCore::isWindowless()
	virtual bool isWindowless() { return false; }

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
	virtual bool onKeyDown(FB::KeyDownEvent* evt, FB::PluginWindow*);
	virtual bool onKeyUp(FB::KeyUpEvent* evt, FB::PluginWindow*);
	virtual bool onMouseDown(FB::MouseDownEvent* evt, FB::PluginWindow*);
	virtual bool onMouseUp(FB::MouseUpEvent* evt, FB::PluginWindow*);
	virtual bool onMouseMove(FB::MouseMoveEvent* evt, FB::PluginWindow*);
	virtual bool onWindowAttached(FB::AttachedEvent* evt, FB::PluginWindow*);
	virtual bool onWindowDetached(FB::DetachedEvent* evt, FB::PluginWindow*);
	/** END EVENTDEF -- DON'T CHANGE THIS LINE **/

	void BootstrapGame(FB::PluginWindow*);
};


#endif

