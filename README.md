# q3plugin

  Simple cross-browser, cross-platform plugin bringing the quake3 engine to the browser.

## Installation

  The compilation process is still rough and has only been tested on Firefox and Chrome on Ubuntu 10.04.

  To get started, you'll need to install cmake, sdl and gtk:

    $ sudo apt-get install cmake libsdl1.2-dev libgtk2.0-dev

  Next, this general series of commands will get you built:

    $ git clone git@github.com:inolen/q3plugin.git
    $ cd q3plugin
    $ git submodule update --recursive --init
    $ ./firebreath/prepmake.sh plugin
    $ svn co svn://svn.icculus.org/quake3/trunk ioquake3
    $ make

  Now, you should have the main plugin shared libraries, libq3plugshim.so and npq3plugin.so in firebreath/build/bin/q3plugin and the ioquake3 executable in ioquake3/build/release-linux-$ARCH/ioquake3.$ARCH (note, when you copy this file please leave off the architecture extension).

  You'll need to copy these 3 files to your firefox plugin directory (which Chrome actually checks as well), ~/.mozilla/plugins. If the directory doesn't already exist go ahead and create it.

  If all is well, you should now be able load up the web/index.html in your web browser and see quake3 running.