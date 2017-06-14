#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

#define IN_CHUNK_SHELL ". ~/.bash_profile;onstat -d"

char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];
char buf[BUFFER_SIZE]={0};

char id[ID_SIZE]={0};
const char *netType = "PF-DB-INFORMIX-CHUNK";
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
    char freerate[20]={0};
    int ret, line=0;
    int flag = 0, free_size, total_size;
    
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(IN_CHUNK_SHELL, "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        if(strstr(buf, "pathname") != NULL)
        {
            flag = 1;
            continue;
        }
        if(strstr(buf, "active") != NULL)
        {
            flag = 0;
            continue;
        }
        if(flag)
        {
            memset(freerate, 0, 20);
            memset(array_data, 0, 100 * 1024);
            ret = split(buf, seg, array_data);
            if(ret >= 8)
            {   
                sscanf(array_data[4], "%d", &total_size);
                sscanf(array_data[5], "%d", &free_size);
                sprintf(freerate, "%.2f", (float)free_size/total_size);
                if(line == 0)
                {
                    sprintf(json_obj,"{\"values\":{\"PF_DB_CHUNK_CHK\":\"%s/%s\",\"PF_DB_CHUNK_OFFSET\":\"%s\",\"PF_DB_CHUNK_SIZE\":\"%s\",\"PF_DB_CHUNK_FREESIZE\":\"%s\",\"PF_DB_CHUNK_FREERATE\":\"%s\",\"PF_DB_CHUNK_FLAG\":\"%s\",\"PF_DB_CHUNK_PATHNAME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", array_data[1],array_data[2] ,array_data[3],array_data[4],array_data[5],freerate,array_data[6],array_data[7],netType,neTopType,id,array_data[7]);
                }
                else
                {
                    sprintf(json_obj,"%s,{\"values\":{\"PF_DB_CHUNK_CHK\":\"%s/%s\",\"PF_DB_CHUNK_OFFSET\":\"%s\",\"PF_DB_CHUNK_SIZE\":\"%s\",\"PF_DB_CHUNK_FREESIZE\":\"%s\",\"PF_DB_CHUNK_FREERATE\":\"%s\",\"PF_DB_CHUNK_FLAG\":\"%s\",\"PF_DB_CHUNK_PATHNAME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", json_obj,array_data[1],array_data[2] ,array_data[3],array_data[4],array_data[5],freerate,array_data[6],array_data[7],netType,neTopType,id,array_data[7]);
                }
                line++;
            }
        }
    }
    printf("%s\n",json_obj);
    fclose(fp);
    fp = NULL;
    return 0;
}