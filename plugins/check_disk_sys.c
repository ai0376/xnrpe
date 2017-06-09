#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

#define DF_SHELL "df -k"

char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];
char buf[BUFFER_SIZE]={0};

char id[ID_SIZE]={0};
const char *netType = "PF-SERVER-UNIX-DOCSYS";
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
    char str[512]={0};
    int ret, line=0,flag=0;
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(DF_SHELL, "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        if(strstr(buf, "Filesystem") != NULL)
        {
            flag = 1;
            continue;
        }
        if(flag)
        {
            memset(array_data, 0, 100 * 1024);
            memset(str , 0 , 512);
            ret = split(buf, seg, array_data);
            if(ret < 5)
            {
                continue;
            }
            sscanf(array_data[4], "%[0-9]", str);
            if(line == 0)
            {
                sprintf(json_obj,"{\"values\":{\"PF_SERVER_SIZE\":\"%s\",\"PF_SERVER_USEDSIZE\":\"%s\",\"PF_SERVER_FREESIZE\":\"%s\",\"PF_SERVER_USEDRATE\":\"%s\",\"PF_SERVER_DOC_FILENAME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", array_data[1],array_data[2],array_data[3],str,array_data[5],netType,neTopType,id,array_data[5]);
            }
            else
            {
                sprintf(json_obj,"%s,{\"values\":{\"PF_SERVER_SIZE\":\"%s\",\"PF_SERVER_USEDSIZE\":\"%s\",\"PF_SERVER_FREESIZE\":\"%s\",\"PF_SERVER_USEDRATE\":\"%s\",\"PF_SERVER_DOC_FILENAME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", json_obj,array_data[1],array_data[2],array_data[3],str,array_data[5],netType,neTopType,id,array_data[5]);
            }
            line++;
        }
    }
    printf("%s\n",json_obj);
    pclose(fp);
    fp=NULL;
    return 0;
}


