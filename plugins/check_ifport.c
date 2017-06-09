#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

#define IF_IOFO_SHELL "netstat -in | grep -v Kernel"
#define IF_STAT_SHELL "ifconfig"

char buf[BUFFER_SIZE]={0};
char if_buf[BUFFER_SIZE]={0};
char if_bufs[BUFFER_SIZE]={0};

char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];

char id[ID_SIZE]={0};
const char *netType = "PF-SERVER-UNIX-PORT";
const char *neTopType ="PF-SERVER-UNIX";
char *neName=NULL;

void str_trim_crlf(char *str)
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

int split(char *str, char *seg, char array[][1024])
{
    int i=0;
    str_trim_ht(str);
    char *substr = strtok(str, seg);
    while(substr != NULL)
    {
        str_trim_crlf(substr);
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
    char seg[10] = {" "};
    char if_name[512]={0};
    int ret, line=0,flag=0;
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(IF_IOFO_SHELL, "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        if(strstr(buf, "Iface") != NULL)
        {
            flag = 1;
            continue;
        }
        if(flag)
        {
            char port_stat_str[30];
            memset(port_stat_str, 0 , 30);
            memset(array_data, 0, 100 * 1024);
            memset(if_name , 0 , 512);
            ret = split(buf, seg, array_data);
            sprintf(if_name, "%s", array_data[0]);

            //exe ifconfig
            {
                char command[512]={0};
                sprintf(command, "%s %s",IF_STAT_SHELL, if_name);
                FILE *fp1 = popen(command, "r");
                if(fp1 == NULL)
                {
                    sprintf(buf,"error:%s\n",strerror(errno));
                    fprintf(stdout,"%s\n",buf);
                    return -1;
                }
                memset(if_buf,0, BUFFER_SIZE);
                memset(if_bufs,0, BUFFER_SIZE);
                while(fgets(if_buf, BUFFER_SIZE,fp1)!= NULL)
                {
                    strcat(if_bufs, if_buf);
                }
                fclose(fp1);
                fp1=NULL;
                if(strstr(if_bufs, "UP") != NULL)
                {
                    sprintf(port_stat_str, "UP");
                }
                else
                {
                    sprintf(port_stat_str, "DOWN");
                }
            }
            if(line == 0)
            {
                sprintf(json_obj,"{\"values\":{\"PF_SERVER_PORT_PORTNAME\":\"%s\",\"PF_SERVER_IPKTS\":\"%s\",\"PF_SERVER_IERRS\":\"%s\",\"PF_SERVER_OPKTS\":\"%s\",\"PF_SERVER_OERRS\":\"%s\",\"PF_SERVER_PORT_PORTSTATE\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", if_name,array_data[3],array_data[4],array_data[7],array_data[8],port_stat_str,netType,neTopType,id,if_name);
            }
            else
            {
                sprintf(json_obj,"%s,{\"values\":{\"PF_SERVER_PORT_PORTNAME\":\"%s\",\"PF_SERVER_IPKTS\":\"%s\",\"PF_SERVER_IERRS\":\"%s\",\"PF_SERVER_OPKTS\":\"%s\",\"PF_SERVER_OERRS\":\"%s\",\"PF_SERVER_PORT_PORTSTATE\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", json_obj,if_name,array_data[3],array_data[4],array_data[7],array_data[8],port_stat_str,netType,neTopType,id,if_name);
            }
            line++;
        }
    }
    printf("%s\n",json_obj);
    pclose(fp);
    fp=NULL;
    return 0;
}