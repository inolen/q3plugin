#ifndef Q3PLUGINWIN_H
#define Q3PLUGINWIN_H

#include "../Q3Plugin.h"

class Q3PluginWin : public Q3Plugin {
public:
	Q3PluginWin();
	virtual ~Q3PluginWin();

protected:
	virtual void ProcessMessage(message_t& msg);

	virtual bool onKeyDown(FB::KeyDownEvent *evt, FB::PluginWindow *window);
	virtual bool onKeyUp(FB::KeyUpEvent *evt, FB::PluginWindow *window);
	virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *window);
	virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *window);
	virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *window);

private:
	bool mouseLocked_;
};

#endif