#/**********************************************************\
#
# Auto-Generated Plugin Configuration file
# for Q3Plugin
#
#\**********************************************************/

set(PLUGIN_NAME "q3plugin")
set(PLUGIN_PREFIX "Q3P")
set(COMPANY_NAME "inolen")

# ActiveX constants:
set(FBTYPELIB_NAME q3pluginLib)
set(FBTYPELIB_DESC "q3plugin 1.0 Type Library")
set(IFBControl_DESC "q3plugin Control Interface")
set(FBControl_DESC "q3plugin Control Class")
set(IFBComJavascriptObject_DESC "q3plugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "q3plugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "q3plugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID a0e98835-c19d-5439-adf5-634c55193122)
set(IFBControl_GUID 3d2312c8-2a15-55b5-8b91-b0367f098dc3)
set(FBControl_GUID 9f9cd2da-ee0b-5395-a5d4-098a4e084a6c)
set(IFBComJavascriptObject_GUID b63554e3-58c5-5e83-b22b-4a3e126cc6de)
set(FBComJavascriptObject_GUID 8f9600e9-fa72-52f7-818b-411024641f21)
set(IFBComEventSource_GUID 29d05903-bf17-52cd-9ca5-9caf9cc5e07a)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "inolen.q3plugin")
set(MOZILLA_PLUGINID "inolen.com/q3plugin")

# strings
set(FBSTRING_CompanyName "inolen")
set(FBSTRING_FileDescription "Do you see any Teletubbies in here? Do you see a slender plastic tag clipped to my shirt with my name printed on it? Do you see a little Asian child with a blank expression on his face sitting outside on a mechanical helicopter that shakes when you put quarters in it? No? Well, that's what you see at a toy store. And you must think you're in a toy store, because you're here shopping for an infant named Jeb.")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2012 inolen")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "q3plugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "q3plugin")
set(FBSTRING_MIMEType "application/x-q3plugin")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)
