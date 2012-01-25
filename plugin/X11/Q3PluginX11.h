#ifndef Q3PLUGIN_X11_H
#define Q3PLUGIN_X11_H

#include "../Q3Plugin.h"

class Q3PluginX11 : public Q3Plugin {
public:
	Q3PluginX11() {};
	virtual ~Q3PluginX11() {};

protected:
	virtual void LaunchGame(FB::PluginWindow* window);
	virtual void CenterMouse(FB::PluginWindow* window);
};

#endif