#ifndef GAMEPROCESSWIN_H
#define GAMEPROCESSWIN_H

#include "../GameProcess.h"

class GameProcessWin : public GameProcess {
public:
	GameProcessWin(FB::PluginWindow *window, const std::string& path);
	virtual ~GameProcessWin();

protected:
	virtual bool SpawnNativeProcess();
	virtual void KillNativeProcess();

private:
	//pid_t gamepid_;
};

#endif