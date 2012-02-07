#ifndef GAMEPROCESSX11_H
#define GAMEPROCESSX11_H

#include "../GameProcess.h"

class GameProcessX11 : public GameProcess {
public:
	GameProcessX11(FB::PluginWindow *window, const std::string& path);
	virtual ~GameProcessX11();

protected:
	virtual bool SpawnNativeProcess();
	virtual void KillNativeProcess();

private:
	pid_t gamepid_;
};

#endif