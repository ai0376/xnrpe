#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern int errno;
#define CPU_SHELL "vmstat 1 2| tail -n 1" // linux cpus
#define  BUF_SIZE  20480
#define ID_SIZE 50

char id[ID_SIZE]={0};
char *netType = "PF-SERVER-UNIX-CPUPFM";
char *neTopType ="PF-SERVER-UNIX";
char *neName="CPU";

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
    char buf[BUFSIZ]={0};
    char json_obj[BUFSIZ]={0};
    char json_array[BUFSIZ]={0};
    char array_data[100][100];
    char seg[10]={" "};
    char str[512]={0};
    int flag = 0,line=0, ret=0;
    char cpu_usedrate[10];
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(CPU_SHELL,"r");
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
                memset(cpu_usedrate, 0, 10);
               
                sprintf(cpu_usedrate,"%d",(100-atoi(array_data[14])));
                if(line == 0)
                {
                    sprintf(json_obj,"{\"PF_SERVER_CPUPFM_FREECPU\":\"%s\",\"PF_SERVER_CPUPFM_SYSCPURATE\":\"%s\",\"PF_SERVER_CPUPFM_USERCPURATE\":\"%s\",\"PF_SERVER_CPUPFM_WAITIOCPU\":\"%s\",\"PF_SERVER_CPUPFM_USEDRATE\":\"%s\",\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", array_data[14],array_data[13],array_data[12],array_data[15],cpu_usedrate,netType,neTopType,id,neName);
                }
                else
                {
                    sprintf(json_obj,"%s,{\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\",\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",json_obj,array_data[14],array_data[13],array_data[12],array_data[15],cpu_usedrate,netType,neTopType,id,neName);
                }
                line++;
            }
        }
    }
    if(line != 0)
    {
        sprintf(json_array, "[%s]",json_obj);
    }
    printf("%s\n",json_array);
    pclose(fp);
    fp=NULL;
    return 0;
}