#ifndef SHIM_H
#define SHIM_H

extern char *GetCurrentProcessPath();
extern void *GetFunctionAddress(const char* name);

extern void Shim_Initialize();
extern void Shim_Shutdown();

#endif