#include <stdio.h>
#include <string.h>
#include <errno.h>
extern int errno;
#define DISK_IO_SHELL "iostat -x"
#define  BUF_SIZE  20480
#define ID_SIZE 50

char id[ID_SIZE]={0};
char *progname = "check_disk_io";

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
    char disk_iobytes[10],disk_avgnum[10];
    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(DISK_IO_SHELL,"r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        if(strstr(buf, "Device") != NULL)
        {
            flag = 1;
            continue;
        }
        if(flag)
        {
            if(buf[0] != 0x0A)
            {
                memset(array_data, 0, 100 * 100);
                ret = split(buf, seg, array_data);
                sprintf(disk_iobytes, "%.2f",(float)(atof(array_data[5])+atof(array_data[6])));
                sprintf(disk_avgnum, "%.2f",(float)(atof(array_data[3])+atof(array_data[4])));
               /* printf("k:%d\n",k);
                int j;
                for(j= 0; j < k; j++)
                {
                    printf("%s\t",array[j]);
                }
                printf("\n");*/
                if(line == 0)
                {
                    sprintf(json_obj,"{\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\",\"name\":\"%s\",\"id\":\"%s\"}", disk_avgnum,array_data[7],disk_iobytes,array_data[11],array_data[0],progname,id);
                }
                else
                {
                    sprintf(json_obj,"%s,{\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\",\"name\":\"%s\",\"id\":\"%s\"}",json_obj,disk_avgnum,array_data[7],disk_iobytes,array_data[11],array_data[0],progname,id);
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
