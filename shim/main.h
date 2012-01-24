#ifndef MAIN_H
#define MAIN_H

#include <dlfcn.h>
#include "../lib/msgpipe.h"

#define FN(ptr,type,name,args) ptr = (type (*)args)dlsym (RTLD_NEXT, name)

extern msgpipe* g_pipe;

extern void process_message(msgpipe_msg*);

#endif