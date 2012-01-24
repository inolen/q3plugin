/**********************************************************\

  Auto-generated Q3PluginApi.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "Q3PluginApi.h"

///////////////////////////////////////////////////////////////////////////////
/// @fn Q3PluginApi::Q3PluginApi(const Q3PluginPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
Q3PluginApi::Q3PluginApi(const Q3PluginPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
{
    registerMethod("echo",      make_method(this, &Q3PluginApi::echo));
    registerMethod("testEvent", make_method(this, &Q3PluginApi::testEvent));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &Q3PluginApi::get_testString,
                        &Q3PluginApi::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &Q3PluginApi::get_version));
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Q3PluginApi::~Q3PluginApi()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
Q3PluginApi::~Q3PluginApi()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Q3PluginPtr Q3PluginApi::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
Q3PluginPtr Q3PluginApi::getPlugin()
{
    Q3PluginPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}



// Read/Write property testString
std::string Q3PluginApi::get_testString()
{
    return m_testString;
}
void Q3PluginApi::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string Q3PluginApi::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}

// Method echo
FB::variant Q3PluginApi::echo(const FB::variant& msg)
{
    static int n(0);
    fire_echo(msg, n++);
    return msg;
}

void Q3PluginApi::testEvent(const FB::variant& var)
{
    fire_fired(var, true, 1);
}

