#include "Q3PluginApi.h"
#include <boost/algorithm/string.hpp>
#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"
#include "ServerCommands.h"

///////////////////////////////////////////////////////////////////////////////
// /@fn Q3PluginApi::Q3PluginApi(const Q3PluginPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
Q3PluginApi::Q3PluginApi(const Q3PluginPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host) {
	registerMethod("connect",       make_method(this, &Q3PluginApi::connect));
	registerMethod("getAllServers", make_method(this, &Q3PluginApi::getAllServers));
	registerMethod("getServerInfo", make_method(this, &Q3PluginApi::getServerInfo));
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Q3PluginApi::~Q3PluginApi()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
Q3PluginApi::~Q3PluginApi() {
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Q3PluginPtr Q3PluginApi::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
Q3PluginPtr Q3PluginApi::getPlugin() {
	Q3PluginPtr plugin(m_plugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

void Q3PluginApi::connect(const std::string& address, const unsigned short port) {
	std::ostringstream server;
	server << address << ":" << port;

	getPlugin()->LaunchGame(server.str());
}

void Q3PluginApi::getAllServers_thread(const std::string& address, const unsigned short port, const FB::JSObjectPtr& callback) {
	// Get the list of servers.
	std::vector<ServerCommands::ServerAddress> servers;

	try {
		ServerCommands::GetAllServers(address, port, servers);
	}
	catch (std::exception& e) {
	}

	// Convert to an FB::VariantList/FB::VariantMap.
	FB::VariantList serversVar;

	for (std::vector<ServerCommands::ServerAddress>::iterator it = servers.begin(); it != servers.end(); ++it) {
		ServerCommands::ServerAddress sa = (*it);

		FB::VariantMap addressVar;
		addressVar["address"] = sa.address().to_string();
		addressVar["port"] = sa.port();
		serversVar.push_back(addressVar);
	}

	// Callback to JS.
	callback->InvokeAsync("", FB::variant_list_of (NULL) (serversVar));
}

void Q3PluginApi::getAllServers(const std::string& address, const unsigned short port, const FB::JSObjectPtr& callback) {
	boost::thread t(boost::bind(&Q3PluginApi::getAllServers_thread, this, address, port, callback));
}

void Q3PluginApi::getServerInfo_thread(const std::string& address, const unsigned short port, const FB::JSObjectPtr& callback) {
	ServerCommands::ServerInfo info;
	int num_retries = 2;

	// Send the message twice in case one is dropped.
	for (int i = 0; i < num_retries; i++) {
		try {
			ServerCommands::GetServerInfo(address, port, info);
		}
		catch (std::exception& e) {
			if (i == num_retries - 1) {
				callback->InvokeAsync("",  FB::variant_list_of (e.what()));
				return;
			}
		}
	}

	// Convert to an FB::VariantMap
	FB::VariantMap infoVar;
	infoVar["hostname"] = info["hostname"];
	infoVar["game"] = info["game"];
	infoVar["gametype"] = info["gametype"];
	infoVar["mapname"] = info["mapname"];
	infoVar["clients"] = info["clients"];
	infoVar["maxclients"] = info["sv_maxclients"];

	// Callback to JS.
	callback->InvokeAsync("",  FB::variant_list_of (NULL) (infoVar));
}

void Q3PluginApi::getServerInfo(const std::string& address, const unsigned short port, const FB::JSObjectPtr& callback) {
	boost::thread t(boost::bind(&Q3PluginApi::getServerInfo_thread, this, address, port, callback));
}