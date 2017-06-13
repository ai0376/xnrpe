#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <map>
using namespace std;

extern int errno;

#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

#define IN_DBSPACE_SHELL ". ~/.bash_profile;onstat -d"

typedef struct{
    std::string dbs_number;
    std::string dbs_name;
    std::string dbs_owner;
    std::string dbs_fchunk;
    std::string dbs_flag;
}type_dbs;

typedef struct{
    std::string chunk_size;
    std::string chunk_free;
    std::string chunk_userate;
}type_chunk;

std::map<string, type_chunk> chunk_ids;
std::map<string, type_dbs> dbs_ids;

char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];
char buf[BUFFER_SIZE]={0};

char id[ID_SIZE]={0};
const char *netType = "PF-DB-INFORMIX-DBS";
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
    char freerate[20]={0};
    int ret, line=0;
    int flag_dbs = 0, flag_chunk=0,free_size, total_size;
    
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(IN_DBSPACE_SHELL, "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        if(strstr(buf, "owner") != NULL)
        {
            flag_dbs = 1;
            flag_chunk = 0;
            continue;
        }
        if(strstr(buf, "pathname") != NULL)
        {
            flag_dbs = 0;
            flag_chunk = 1;
            continue;
        }
        
        if(strstr(buf, "active") != NULL)
        {
            flag_dbs = 0;
            flag_chunk = 0;
            continue;
        }
        if(flag_dbs)
        {
            memset(array_data, 0, 100 * 1024);
            ret = split(buf, seg, array_data);
            if(ret >= 10)
            {
                type_dbs dbs_info;
                dbs_info.dbs_number = array_data[1];
                dbs_info.dbs_flag = array_data[2];
                dbs_info.dbs_fchunk = array_data[3];
                dbs_info.dbs_owner = array_data[8];
                dbs_info.dbs_name = array_data[9];
                dbs_ids[array_data[1]] = dbs_info;
            }
        }
        if(flag_chunk)
        {
            memset(array_data, 0, 100 * 1024);
            ret = split(buf, seg, array_data);
            if(ret >= 8)
            {
                type_chunk chunk_info;
                char userate[10]={0};
                chunk_info.chunk_size = array_data[4];
                chunk_info.chunk_free = array_data[5];
                sscanf(array_data[4], "%d", &total_size);
                sscanf(array_data[5], "%d", &free_size);
                sprintf(userate, "%.2f", (float)(total_size-free_size)/total_size);
                chunk_info.chunk_userate = userate;
                chunk_ids[array_data[2]]=chunk_info;               
            }
        }
    }
    std::map<string,type_dbs>::iterator iter = dbs_ids.begin();
    for(; iter!= dbs_ids.end(); iter++)
    {
        type_chunk chunk_info;
        type_dbs dbs_info;
        std::string dbs_id = iter->first;
        dbs_info = iter->second;
        std::map<string,type_chunk>::iterator u_iter = chunk_ids.find(dbs_id);
        if(u_iter != chunk_ids.end())
        {
            chunk_info = u_iter->second;
        }

        if(line == 0)
        { 
            sprintf(json_obj,"{\"values\":{\"PF_DB_DBS_NUMBER\":\"%s\",\"PF_DB_DBS_NAME\":\"%s\",\"PF_DB_DBS_OWNER\":\"%s\",\"PF_DB_DBS_FLAG\":\"%s\",\"PF_DB_DBS_FCHUNK\":\"%s\",\"PF_DB_DBS_CHUNKSIZE\":\"%s\",\"PF_DB_DBS_CHUNKFREESIZE\":\"%s\",\"PF_DB_DBS_USEDRATE\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", dbs_id.c_str(),dbs_info.dbs_name.c_str() ,dbs_info.dbs_owner.c_str(),dbs_info.dbs_flag.c_str(),dbs_info.dbs_fchunk.c_str(),chunk_info.chunk_size.c_str(),chunk_info.chunk_free.c_str(),chunk_info.chunk_userate.c_str(),netType,neTopType,id,dbs_info.dbs_name.c_str());
        }
        else
        {
            sprintf(json_obj,"%s,{\"values\":{\"PF_DB_DBS_NUMBER\":\"%s\",\"PF_DB_DBS_NAME\":\"%s\",\"PF_DB_DBS_OWNER\":\"%s\",\"PF_DB_DBS_FLAG\":\"%s\",\"PF_DB_DBS_FCHUNK\":\"%s\",\"PF_DB_DBS_CHUNKSIZE\":\"%s\",\"PF_DB_DBS_CHUNKFREESIZE\":\"%s\",\"PF_DB_DBS_USEDRATE\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", json_obj, dbs_id.c_str(),dbs_info.dbs_name.c_str() ,dbs_info.dbs_owner.c_str(),dbs_info.dbs_flag.c_str(),dbs_info.dbs_fchunk.c_str(),chunk_info.chunk_size.c_str(),chunk_info.chunk_free.c_str(),chunk_info.chunk_userate.c_str(),netType,neTopType,id,dbs_info.dbs_name.c_str());
        }
        line++;
    }
     
    printf("%s\n",json_obj);
    fclose(fp);
    fp = NULL;
    return 0;
}