#include <stdio.h>
#include <string.h>
#include <errno.h>
extern int errno;
#define DISK_IO_SHELL "iostat -x"
#define  BUF_SIZE  20480

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
int main(int argc,char **argv)
{
    FILE *fp = NULL;
    char buf[BUFSIZ]={0};
    char json_obj[BUFSIZ]={0};
    char array[BUFSIZ]={0};

    char str[512]={0};
    int flag = 0,line=0;
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
                char array[100][100];
                char seg[10]={" "};
                int k = split(buf, seg, array);
                float disk_iobytes,disk_avgnum;

                disk_iobytes = atof(array[5])+atof(array[6]);
                disk_avgnum = atof(array[3])+atof(array[4]);
               /* printf("k:%d\n",k);
                int j;
                for(j= 0; j < k; j++)
                {
                    printf("%s\t",array[j]);
                }
                printf("\n");*/
               // sprintf(json_obj,"\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",,\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\"", ,array[9],array[0]);
            }
        }
    }
    pclose(fp);
    fp=NULL;
    return 0;
}
