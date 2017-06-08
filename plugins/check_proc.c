#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <map>
#include <vector>
using namespace std;

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
const char *netType = "PF-SERVER-UNIX-PROC";
const char *neTopType ="PF-SERVER-UNIX";
char *neName=NULL;

typedef struct {
    std::string pf_server_proc_id;
    std::string pf_server_proc_name;
    std::string pf_server_proc_cpuused;
    std::string pf_server_proc_memused;
    int pf_server_proc_stat;
    std::string pf_server_proc_user;
    std::string pf_server_proc_para;
    std::string neType;
    std::string neTopType;
    std::string neId;
    std::string neName;
}type_proc_info;

std::map<string, string> map_process_base;
std::map<string, string> map_process_cur;

std::vector<type_proc_info> proc_infos;

size_t cxxsplit(std::string &src, std::vector<std::string> *tokens, std::string sep)
{
	size_t last= 0;
	size_t index = src.find(sep, last);
	size_t length = src.size();
	while(index != std::string::npos)
	{
		tokens->push_back(src.substr(last, index-last));
		last = index + 1;
		index = src.find(sep, last);
	}
	if(length - last > 0)
	{
		tokens->push_back(src.substr(last, length-last));
	}
	return tokens->size();
}

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

void read_tmp_data()
{
    FILE *fp = NULL;
    char buf[BUF_SIZE]={0};
    char *varname;
    char *varvalue;
    fp = fopen("data.tmp", "a+");
    if(fp == NULL)
    {
        fprintf(stdout,"read_tmp_data open file failed\n");
        return;
    }
    while(fgets(buf, BUF_SIZE, fp))
    {
        varname = strtok(buf, "=");
        varvalue = strtok(NULL, "\n");
        map_process_base[varname]=varvalue;
        memset(buf, 0, BUF_SIZE);
    }
    fclose(fp);
    fp = NULL;
}

void write_tmp_data()
{
    FILE *fp = NULL;
    fp = fopen("data.tmp","w");
    if(fp == NULL)
    {
        fprintf(stdout,"write_tmp_data open file failed\n");
        return;
    }
    std::map<string,string>::iterator iter= map_process_cur.begin();
    for(; iter != map_process_cur.end(); iter++)
    {
        std::string str;
        str.append(iter->first);
        str.append("=");    
        str.append(iter->second);
        str.append("\n");
        fputs(str.c_str(),fp);
        fflush(fp);
    }
    fclose(fp);
    fp = NULL;
    return ;
}

int main(int argc,char **argv)
{

    FILE *fp = NULL;
    char seg[10]={" "};
    char str[512]={0};
    char proc_para[512]={0};
    int  ret=0;
    int line=0;
    int state;
    
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
    read_tmp_data();
    memset(json_obj, 0, BUFFER_SIZE);
    while(fgets(buf, BUF_SIZE, fp) != NULL)
    {
        memset(array_data, 0, 100*1024);
        ret = split(buf, seg, array_data);
        if(ret != 9)
            continue;
        
        if((strcmp(array_data[1],"scf")== 0) && (strcmp(array_data[6],"scf")== 0))
        {
            type_proc_info proc_info;
            string key, value;
            bzero(str,512);
            bzero(proc_para,512);
            sprintf(str, "%s %s %s",array_data[5], array_data[6], array_data[7]);
            sprintf(proc_para,"%s:%s%s%s",array_data[5],array_data[6],array_data[7],array_data[8]);
            key.append(array_data[5]);
            key.append(":");
            key.append(array_data[6]);
            key.append(":");
            key.append(array_data[7]);
            key.append(":");
            key.append(array_data[8]);
            value.append(array_data[0]);
            std::map<string, string>::iterator iter= map_process_base.find(key);
            if(iter != map_process_base.end())
            {
                //find
                std::string pid=iter->second;
                if(pid.compare(value) == 0) //相等
                {
                    map_process_cur[key]=value; //填值
                    state = 1;
                }
                else // 进程重启过
                {
                    map_process_cur[key]=value; //更新值
                    state = 2;
                }
            }
            else  //新加进程
            {
                map_process_cur[key]=value;
                state = 1;
            }
            proc_info.pf_server_proc_id = array_data[0];
            proc_info.pf_server_proc_name = array_data[1];
            proc_info.pf_server_proc_cpuused = array_data[2];
            proc_info.pf_server_proc_memused = array_data[3];
            proc_info.pf_server_proc_stat = state;
            proc_info.pf_server_proc_user = array_data[5];
            proc_info.pf_server_proc_para = proc_para;
            proc_info.neType = netType;
            proc_info.neTopType = neTopType;
            proc_info.neId = id;
            proc_info.neName = str;

            proc_infos.push_back(proc_info);
        }
    }//

    // 判断是否有挂掉进程
    std::map<string, string>::iterator iter= map_process_base.begin(); 
    for(; iter != map_process_base.end(); iter++)
    {
        std::string key = iter->first;
        std::map<string, string>::iterator cur_iter = map_process_cur.find(key);
        if(cur_iter != map_process_cur.end()) //has process
        {

        }
        else //process has down
        {
            std::vector<string> token;
            size_t n = cxxsplit(key, &token,":");
            
            type_proc_info proc_info;
            proc_info.pf_server_proc_id = "0";
            proc_info.pf_server_proc_name = "0";
            proc_info.pf_server_proc_cpuused = "0";
            proc_info.pf_server_proc_memused = "0";
            proc_info.pf_server_proc_stat = 0;
            proc_info.pf_server_proc_user = "0";
            proc_info.pf_server_proc_para = "0";
            proc_info.neType = netType;
            proc_info.neTopType = neTopType;
            proc_info.neId = id;
            std::string  name =token.at(0)+" "+ token.at(1)+" "+token.at(2);
            proc_info.neName = name;
            proc_infos.push_back(proc_info);
        }
    }

    // parase  json data
    if(proc_infos.size() > 0)
    {
        std::vector<type_proc_info>::iterator vter = proc_infos.begin();
        for(; vter != proc_infos.end(); vter++)
        {
            type_proc_info info = (*vter);
            if(line == 0)
            {
                sprintf(json_obj,"{\"values\":{\"PF_SERVER_PROC_ID\":\"%s\",\"PF_SERVER_PROC_NAME\":\"%s\",\"PF_SERVER_PROC_CUPUSED\":\"%s\",\"PF_SERVER_PROC_MEMUSED\":\"%s\",\"PF_SERVER_PROC_STAT\":\"%d\",\"PF_SERVER_PROC_USER\":\"%s\",\"PF_SERVER_PROC_PARA\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", info.pf_server_proc_id.c_str(),info.pf_server_proc_name.c_str(),info.pf_server_proc_cpuused.c_str(),info.pf_server_proc_memused.c_str(),info.pf_server_proc_stat,info.pf_server_proc_user.c_str(),info.pf_server_proc_para.c_str(),info.neType.c_str(),info.neTopType.c_str(),info.neId.c_str(),info.neName.c_str());
            }
            else
            {
                sprintf(json_obj,"%s,{\"values\":{\"PF_SERVER_PROC_ID\":\"%s\",\"PF_SERVER_PROC_NAME\":\"%s\",\"PF_SERVER_PROC_CUPUSED\":\"%s\",\"PF_SERVER_PROC_MEMUSED\":\"%s\",\"PF_SERVER_PROC_STAT\":\"%d\",\"PF_SERVER_PROC_USER\":\"%s\",\"PF_SERVER_PROC_PARA\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",json_obj,info.pf_server_proc_id.c_str(),info.pf_server_proc_name.c_str(),info.pf_server_proc_cpuused.c_str(),info.pf_server_proc_memused.c_str(),info.pf_server_proc_stat,info.pf_server_proc_user.c_str(),info.pf_server_proc_para.c_str(),info.neType.c_str(),info.neTopType.c_str(),info.neId.c_str(),info.neName.c_str());
            }
            line++;
        }
    }
    
    //////////write file/////////////////
    if(map_process_cur.size() > 0)
    {
        write_tmp_data(); //write map_process_cur data;
    }
    if(line != 0)
    {
        printf("%s\n",json_obj);
    }
    pclose(fp);
    fp=NULL;
    return 0;
}
