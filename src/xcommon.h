#ifndef __XCOMMON_H
#define __XCOMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/time.h>

#define true 1
#define false 0

#define OK 0
#define ERROR -1
#define MAX_LISTEN_SOCKS 16
#define NI_MAXHOST 1025
#define DEFAULT_SERVER_PORT  5566
#define DEFAULT_SOCKET_TIMEOUT 60
#define DEFAULT_CONNECTION_TIMEOUT 300
#define EPOLL_LEN 20480
#define MAX_INPUT_BUFFER 2048
#define MAX_FILENAME_LENGTH 256
#define DEFAULT_SOCK_SEND_RECV_TIMEOUT 10 //seconds

extern char server_address[NI_MAXHOST];
extern int server_port;
extern int sock_send_recv_timeout;

typedef struct args{
    int time; //seconds
    char cmd[MAX_INPUT_BUFFER];
}ARGS;

typedef struct command_struct{
    char *command_name;
    char *command_line;
    struct command_struct *next;
}command;

int process_arguments(int argc, char **argv);

int setnonblocking(int sock);
int resolve_host_name(char* hostname, struct in_addr* addr);

int start_connect(char *host, int port);

int report_tcp_information(char *info , int len, int recv_flag);
int send_tcp_all(int s,char *buf, int len);
int send_tcp_and_recv(int s, char *buf, int len);
int pack_msg(char *inbuf, unsigned int len, char *outbuf);


//define in xnrpe.c
void fun_int(int arg);
void fun_term(int arg);
void handle_pipe(int sig);
void *task(void *args);
void usage(int result);
int process_arguments(int argc, char **argv);
int add_command(char *command_name, char *command_line);
command *find_command(char *command_name);
int read_config_file(char *filename);
void free_memory(void);

#endif // __XCOMMON_H
