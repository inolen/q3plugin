#ifndef Q3PLUGINAPI_H
#define Q3PLUGINAPI_H

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "Q3Plugin.h"

class Q3PluginApi : public FB::JSAPIAuto
{
public:
	Q3PluginApi(const Q3PluginPtr& plugin, const FB::BrowserHostPtr& host);
	virtual ~Q3PluginApi();

	Q3PluginPtr getPlugin();

	void connect(const std::string& addr, unsigned short port);
	void getAllServers_thread(const std::string& addr, const unsigned short port, const FB::JSObjectPtr& callback);
	void getAllServers(const std::string& addr, const unsigned short port, const FB::JSObjectPtr& callback);
	void getServerInfo_thread(const std::string& addr, const unsigned short port, const FB::JSObjectPtr& callback);
	void getServerInfo(const std::string& addr, const unsigned short port, const FB::JSObjectPtr& callback);

private:
	Q3PluginWeakPtr m_plugin;
	FB::BrowserHostPtr m_host;
};

#endif
