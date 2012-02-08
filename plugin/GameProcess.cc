#include "GameProcess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GameProcess::GameProcess(FB::PluginWindow *window, const std::string& path) :
	window_(window),
	path_(path) {
}

GameProcess::~GameProcess() {
}

bool GameProcess::Spawn(int width, int height, const std::string& connectTo) {
	int argc = !connectTo.empty() ? 11 : 9;

	argv_ = (char**)malloc(sizeof(char*) * (argc+1));
	memset(argv_, 0, sizeof(char*) * (argc+1));

	argv_[0] = strdup(path_.c_str());
	argv_[1] = strdup("+r_fullscreen");
	argv_[2] = strdup("0");
	argv_[3] = strdup("+r_mode");
	argv_[4] = strdup("-1");
	argv_[5] = strdup("+r_customWidth");
	argv_[6] = (char*)malloc(sizeof(char) * 8);
	sprintf(argv_[6], "%i", width);
	argv_[7] = strdup("+r_customHeight");
	argv_[8] = (char*)malloc(sizeof(char) * 8);
	sprintf(argv_[8], "%i", height);

	if (!connectTo.empty()) {
		argv_[9] = strdup("+connect");
		argv_[10] = strdup(connectTo.c_str());
	}

	// Spawn the native process.
	SpawnNativeProcess();

	return true;
}

void GameProcess::Kill() {
	// Kill the native process.
	KillNativeProcess();

	// Free up arguments.
	if (argv_ != NULL) {
		char** p = argv_;

		while (*p != NULL) {
			free(*p);
			p++;
		}

		free(argv_);

		argv_ = NULL;
	}
}