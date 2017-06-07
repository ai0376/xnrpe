#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;
#define PROCE_SHELL "ps -eo pid,comm,pcpu,pmem,s,user,args | grep \"scf\""
#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

char buf[BUFFER_SIZE]={0};
char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];

char id[ID_SIZE]={0};
char *netType = "PF-SERVER-UNIX-PROC";
char *neTopType ="PF-SERVER-UNIX";
char *neName=NULL;

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
    
    char seg[10]={" "};
    char str[512]={0};
    char proc_para[512]={0};
    int  ret=0;
    int line=0;
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(PROCE_SHELL, "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    memset(json_obj, 0, BUFFER_SIZE);
    while(fgets(buf, BUF_SIZE, fp) != NULL)
    {
        memset(array_data, 0, 100*1024);
        ret = split(buf, seg, array_data);
        if(ret != 9)
            continue;
        
        if((strcmp(array_data[1],"scf")== 0) && (strcmp(array_data[6],"scf")== 0))
        {
            bzero(str,512);
            bzero(proc_para,512);
            sprintf(str, "%s %s",array_data[6], array_data[7]);
            sprintf(proc_para,"%s%s%s",array_data[6],array_data[7],array_data[8]);
            if(line == 0)
            {
                sprintf(json_obj,"{\"values\":{\"PF_SERVER_PROC_ID\":\"%s\",\"PF_SERVER_PROC_NAME\":\"%s\",\"PF_SERVER_PROC_CUPUSED\":\"%s\",\"PF_SERVER_PROC_MEMUSED\":\"%s\",\"PF_SERVER_PROC_STAT\":\"%s\",\"PF_SERVER_PROC_USER\":\"%s\",\"PF_SERVER_PROC_PARA\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",array_data[0],array_data[1],array_data[2],array_data[3],array_data[4],array_data[5],proc_para,netType,neTopType,id,str);
            }
            else
            {
                sprintf(json_obj,"%s,{\"values\":{\"PF_SERVER_PROC_ID\":\"%s\",\"PF_SERVER_PROC_NAME\":\"%s\",\"PF_SERVER_PROC_CUPUSED\":\"%s\",\"PF_SERVER_PROC_MEMUSED\":\"%s\",\"PF_SERVER_PROC_STAT\":\"%s\",\"PF_SERVER_PROC_USER\":\"%s\",\"PF_SERVER_PROC_PARA\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",json_obj,array_data[0],array_data[1],array_data[2],array_data[3],array_data[4],array_data[5],proc_para,netType,neTopType,id,str);
            }
            line++;
        }
    }
    if(line != 0)
    {
        printf("%s\n",json_obj);
    }
    return 0;
}
