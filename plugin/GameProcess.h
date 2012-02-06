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

	bool Launch(int width, int height);

protected:
	virtual bool SpawnNativeProcess() = 0;
	virtual bool KillNativeProcess() = 0;

	FB::PluginWindow *window_;
	std::string path_;
	char **argv_;

private:
	void Shutdown();
};

#endif