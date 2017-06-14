#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

#define IN_BASIC_SHELL ". ~/.bash_profile;onstat - | sed '/^$/d'"
#define IN_NOT_INIT "not initialized"


char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];
char buf[BUFFER_SIZE]={0};

char id[ID_SIZE]={0};
const char *netType = "";
const char *neTopType ="PF-DB-INFORMIX";
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
    char version[512]={0};
    int ret, line=0;
    int pf_db_stat = 0;
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(IN_BASIC_SHELL, "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {   
        memset(array_data, 0, 100 * 1024);
        memset(str , 0 , 512);
        memset(version , 0 , 512);
        if(strstr(buf, IN_NOT_INIT) != NULL)  //not start
        {
            pf_db_stat = 3;
        }
        else // has run 
        {
            if(strstr(buf, "On-Line"))
            {
                pf_db_stat = 0;
            }
            else if(strstr(buf, "Read-Only"))
            {
                pf_db_stat = 1;
            }
            
            ret = split(buf, seg, array_data);
            sprintf(str,"%s:%s",array_data[10],array_data[12]);
            sprintf(version, "%s", array_data[5]);
        }
        if(line == 0)
        {
            sprintf(json_obj,"{\"values\":{\"PF_DB_VERSION\":\"%s\",\"PF_DB_STATE\":\"%d\",\"PF_DB_RUNTIME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", version ,pf_db_stat,str,netType,neTopType,id,"");
        }
        else
        {
            sprintf(json_obj,"%s,{\"values\":{\"PF_DB_VERSION\":\"%s\",\"PF_DB_STATE\":\"%d\",\"PF_DB_RUNTIME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", json_obj,version ,pf_db_stat,str,netType,neTopType,id,"");
        }
        line++;
    }
    printf("%s\n",json_obj);
    pclose(fp);
    fp=NULL;
    return 0;
}