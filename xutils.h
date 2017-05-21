#ifndef __XUTILS_H
#define __XUTILS_H

#include "xconfig.h"

char *strip(char *);
int sendall(int, char *, int *);
int recvall(int ,char *,int *, int);
void open_log_file();
void logit(int priority, const char *format, ...);
void close_log_file();

#endif