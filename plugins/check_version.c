#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern int errno;
#define VERSION_SHELL "cat /proc/cpuinfo  | grep \"physical id\" | sort | uniq | wc -l;cat /proc/meminfo | grep \"MemTotal\" | cut -f2 -d: | awk '{ print $1 }';getconf LONG_BIT;uname -var;uname -n;uname -r"  // linux swap
#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

char id[ID_SIZE]={0};
char *netType = "";
char *neTopType ="PF-SERVER-UNIX";
char *neName="";

void str_trim_crlf(char *str) //去除\r\n
{
	char *p = &str[strlen(str)-1];
	while (*p == '\r' || *p == '\n')
		*p-- = '\0';

}

void str_trim_ht(char *str) //去除\t
{
    char *p = str;
    while(*p != '\0')
    {
        if(*p == '\t')
            *p = '\x20';
        ++p;
    }
}

int split(char *str, char *seg, char array[][100])
{
    int i=0;
    char *substr = strtok(str, seg);
    while(substr != NULL)
    {
        strcpy(array[i],substr);
        substr = strtok(NULL,seg);
        i++;
    }
    return i;
}

int process_arguments(int argc,char **argv)
{
    if(argc != 2)
    {
        fprintf(stdout,"please input $:./exe id\n");
        return -1;
    }
    strcpy(id, argv[1]);
    return 0;
}

int main(int argc,char **argv)
{
    FILE *fp = NULL;
    char buf[BUFFER_SIZE]={0};
    char json_obj[BUFFER_SIZE]={0};
    char dststr[BUFFER_SIZE]={0};
    char array_data[100][100];
    char seg[10]={"\n"};
    char str[512]={0};
    int flag = 0,line=0, ret=0;
    int memsize;
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(VERSION_SHELL,"r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    memset(dststr, 0 , BUFFER_SIZE);
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        strcat(dststr, buf);
    }
    ret = split(dststr, seg, array_data);
    memsize = atoi(array_data[1])/1024;
    sprintf(json_obj,"{\"values\":{\"PF_SERVER_CPUNUM\":\"%s\",\"PF_SERVER_MEMSIZE\":\"%d\",\"PF_SERVER_KERNELTYPE\":\"%s\",\"PF_SERVER_SYSMODEL\":\"%s\",\"PF_SERVER_HOSTNAME\":\"%s\",\"PF_SERVER_OSVERSION\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",array_data[0],memsize, array_data[2],array_data[3],array_data[4],array_data[5],netType,neTopType,id,neName);
    printf("%s\n",json_obj);
    pclose(fp);
    fp=NULL;
    return 0;
}