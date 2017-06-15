#ifndef __XLOG_H
#define __XLOG_H

#include <stdio.h>

//copy from linux header file <syslog.h>
#define XLOG_EMERG       0       /* system is unusable */
#define XLOG_ALERT       1       /* action must be taken immediately */
#define XLOG_CRIT        2       /* critical conditions */
#define XLOG_ERR         3       /* error conditions */
#define XLOG_WARNING     4       /* warning conditions */
#define XLOG_NOTICE      5       /* normal but significant condition */
#define XLOG_INFO        6       /* informational */
#define XLOG_DEBUG       7       /* debug-level messages */

#define LOG_DEFAULT_FILE "/var/log/xnrpe.log"

extern char *log_file;

char *strip(char *buffer);
void open_log_file();
void logit(int priority, const char *format, ...);
void close_log_file();

#endif