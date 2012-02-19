#include "shim.h"
#include <stdio.h>

__attribute__((constructor)) static void load(void) {
	Shim_Initialize();
}

__attribute__((destructor)) static void unload(void) {
	Shim_Shutdown();
}