#ifndef MAIN_H
#define MAIN_H

#include "../lib/msgpipe.h"

extern msgpipe g_pipe;

extern void process_message(msgpipe_msg*);

#endif
