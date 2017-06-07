#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern int errno;
#define MEM_SHELL "free | grep -i \"mem\" | awk '{print \"\"($3-$6-$7)/$2*100\"\"}'" // linux memory
#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

char id[ID_SIZE]={0};
char *netType = "PF-SERVER-UNIX-MEM";
char *neTopType ="PF-SERVER-UNIX";
char *neName="内存";

void str_trim_crlf(char *str) //去除\r\n
{
	char *p = &str[strlen(str)-1];
	while (*p == '\r' || *p == '\n')
		*p-- = '\0';

}

int split(char *str, char *seg, char array[][100])
{
    int i=0;
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
    char buf[BUFFER_SIZE]={0};
    char json_obj[BUFFER_SIZE]={0};
    char json_array[BUFFER_SIZE]={0};
    char array_data[100][100];
    char seg[10]={" "};
    char str[512]={0};
    int flag = 0,line=0, ret=0;
    char mem_usedrate[20];
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(MEM_SHELL,"r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        {
           // if(buf[0] != 0x0A)
            {
                memset(array_data, 0, 100 * 100);
                ret = split(buf, seg, array_data);
                memset(mem_usedrate, 0, 20);
                memcpy(mem_usedrate, array_data[0], strlen(array_data[0])-1);
                if(line == 0)
                {
                    sprintf(json_obj,"{\"values\":{\"PF_SERVER_MEM_MEMUSEDRATE\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",mem_usedrate,netType,neTopType,id,neName);
                }
                else
                {
                    sprintf(json_obj,"%s,{\"values\":{\"PF_SERVER_MEM_MEMUSEDRATE\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",json_obj,mem_usedrate,netType,neTopType,id,neName);
                }
                line++;
            }
        }
    }
    if(line != 0)
    {
        //sprintf(json_array, "[%s]",json_obj);
    }
    printf("%s\n",json_obj);
    pclose(fp);
    fp=NULL;
    return 0;
}