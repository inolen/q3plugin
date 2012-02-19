#include "../shim.h"
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>

char *GetCurrentProcessPath() {
	static char path[PATH_MAX];

	int len = readlink("/proc/self/exe", path, PATH_MAX - 1);

	if (len == -1) {
		return NULL;
	}

	return path;
}

void *GetFunctionAddress(const char* name) {
	return dlsym(RTLD_NEXT, name);
}