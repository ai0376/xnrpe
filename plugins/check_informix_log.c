#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

#define IN_LOGR_SHELL ". ~/.bash_profile;onstat -l | sed -n '/Physical Logging/{ n; n; p; n; n; p; }' | awk '{ printf \"%s \", $0}'"

char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];
char buf[BUFFER_SIZE]={0};

char id[ID_SIZE]={0};
const char *netType = "PF-DB-INFORMIX-LOGR";
const char *neTopType ="PF-DB-INFORMIX";
char *neName="LOGR";

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
    int ret, line=0;
    
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(IN_LOGR_SHELL , "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        memset(array_data, 0, 100 * 1024);
        ret = split(buf, seg, array_data);
        if(ret >= 10)
        {
            if(line == 0)
            {
                
                sprintf(json_obj,"{\"values\":{\"PF_DB_LOGR_ADDRESS\":\"%s\",\"PF_DB_LOGR_NUMBER\":\"%s\",\"PF_DB_LOGR_FLAG\":\"%s\",\"PF_DB_LOGR_UNIQID\":\"%s\",\"PF_DB_LOGR_SIZE\":\"%s\",\"PF_DB_LOGR_USED\":\"%s\",\"PF_DB_LOGR_USED2\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", array_data[0],array_data[1] ,array_data[2],array_data[3],array_data[5],array_data[6],array_data[7],netType,neTopType,id,neName);
            }
            else
            {
                sprintf(json_obj,"%s,{\"values\":{\"PF_DB_LOGR_ADDRESS\":\"%s\",\"PF_DB_LOGR_NUMBER\":\"%s\",\"PF_DB_LOGR_FLAG\":\"%s\",\"PF_DB_LOGR_UNIQID\":\"%s\",\"PF_DB_LOGR_SIZE\":\"%s\",\"PF_DB_LOGR_USED\":\"%s\",\"PF_DB_LOGR_USED2\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", json_obj,array_data[0],array_data[1] ,array_data[2],array_data[3],array_data[5],array_data[6],array_data[7],netType,neTopType,id,neName);
            }
            line++;
        }
    }
    printf("%s\n",json_obj);
    fclose(fp);
    fp = NULL;
    return 0;
}