#include "FactoryBase.h"
#include "Q3Plugin.h"
#include <boost/make_shared.hpp>

#if defined FB_WIN
#include "X11/Q3PluginWin.h"
#define Q3PLUGIN Q3PluginWin
#elif defined FB_MACOSX
#include "X11/Q3PluginMac.h"
#define Q3PLUGIN Q3pluginMac
#else
#include "X11/Q3PluginX11.h"
#define Q3PLUGIN Q3PluginX11
#endif

class PluginFactory : public FB::FactoryBase {
public:
    FB::PluginCorePtr createPlugin(const std::string& mimetype) {
        return boost::make_shared<Q3PLUGIN>();
    }

    void globalPluginInitialize() {
        Q3Plugin::StaticInitialize();
    }

    void globalPluginDeinitialize() {
        Q3Plugin::StaticDeinitialize();
    }
};

FB::FactoryBasePtr getFactoryInstance() {
    static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
    return factory;
}

