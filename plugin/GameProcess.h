#ifndef GAMEPROCESS_H
#define GAMEPROCESS_H

#include <string>

namespace FB {
class PluginWindow;
}

class GameProcess {
public:
	GameProcess(FB::PluginWindow *window, const std::string& path);
	virtual ~GameProcess();

	bool Spawn(int width, int height, const std::string& connectTo);
	void Kill();

protected:
	virtual bool SpawnNativeProcess() = 0;
	virtual void KillNativeProcess() = 0;

	FB::PluginWindow *window_;
	std::string path_;
	char **argv_;
};

#endif