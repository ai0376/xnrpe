#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DISK_IO_SHELL "iostat -x"
#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

extern int errno;

char buf[BUFFER_SIZE]={0};
char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][100];

char id[ID_SIZE]={0};
char *netType = "PF-SERVER-UNIX-DISKIO";
char *neTopType ="PF-SERVER-UNIX";

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
    
    char seg[10]={" "};
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
                memset(disk_avgnum, 0, 10);
                memset(disk_iobytes, 0, 10);
                sprintf(disk_iobytes, "%.2f",(atof(array_data[5])+atof(array_data[6])));
                sprintf(disk_avgnum, "%.2f",(atof(array_data[3])+atof(array_data[4])));
                if(line == 0)
                {
                    sprintf(json_obj,"{\"values\":{\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", disk_avgnum,array_data[11],disk_iobytes,array_data[9],array_data[0],netType,neTopType,id,array_data[0]);
                }
                else
                {
                    sprintf(json_obj,"%s,{\"values\":{\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}",json_obj,disk_avgnum,array_data[11],disk_iobytes,array_data[9],array_data[0],netType,neTopType,id,array_data[0]);
                }
                line++;
            }
        }
    }
    if(line != 0)
    {
       // sprintf(json_array, "[%s]",json_obj);
    }
    printf("%s\n",json_obj);
    pclose(fp);
    fp=NULL;
    return 0;
}

/*Device:         rrqm/s   wrqm/s   r/s   w/s   rsec/s   wsec/s avgrq-sz avgqu-sz   await svctm %util
sda               0.00     3.50 0.40 2.50     5.60    48.00    18.48     0.00    0.97   0.97   0.28
sdb               0.00     0.00 0.00 0.00     0.00     0.00     0.00     0.00    0.00   0.00   0.00
sdc               0.00     0.00 0.00 0.00     0.00     0.00     0.00     0.00    0.00   0.00   0.00
sdd               0.00     0.00 0.00 0.00     0.00     0.00     0.00     0.00    0.00   0.00   0.00
sde               0.00     0.10 0.30 0.20     2.40     2.40     9.60     0.00    1.60   1.60   0.08
sdf              17.40     0.50 102.00 0.20 12095.20     5.60   118.40     0.70    6.81   2.09 21.36
sdg             232.40     1.90 379.70 0.50 76451.20    19.20   201.13     4.94   13.78   2.45 93.16

rrqm/s:   每秒进行 merge 的读操作数目。即 delta(rmerge)/s
wrqm/s: 每秒进行 merge 的写操作数目。即 delta(wmerge)/s
r/s:           每秒完成的读 I/O 设备次数。即 delta(rio)/s
w/s:         每秒完成的写 I/O 设备次数。即 delta(wio)/s
rsec/s:    每秒读扇区数。即 delta(rsect)/s
wsec/s: 每秒写扇区数。即 delta(wsect)/s
rkB/s:     每秒读K字节数。是 rsect/s 的一半，因为每扇区大小为512字节。(需要计算)
wkB/s:    每秒写K字节数。是 wsect/s 的一半。(需要计算)
avgrq-sz:平均每次设备I/O操作的数据大小 (扇区)。delta(rsect+wsect)/delta(rio+wio)
avgqu-sz:平均I/O队列长度。即 delta(aveq)/s/1000 (因为aveq的单位为毫秒)。
await:    平均每次设备I/O操作的等待时间 (毫秒)。即 delta(ruse+wuse)/delta(rio+wio)
svctm: 平均每次设备I/O操作的服务时间 (毫秒)。即 delta(use)/delta(rio+wio)
%util:     一秒中有百分之多少的时间用于 I/O 操作，或者说一秒中有多少时间 I/O 队列是非空的。即 delta(use)/s/1000 (因为use的单位为毫秒)

如果 %util 接近 100%，说明产生的I/O请求太多，I/O系统已经满负荷，该磁盘
可能存在瓶颈。
idle小于70% IO压力就较大了,一般读取速度有较多的wait.
同时可以结合vmstat 查看查看b参数(等待资源的进程数)和wa参数(IO等待所占用的CPU时间的百分比,高过30%时IO压力高)

avg-cpu段:
%user: 在用户级别运行所使用的CPU的百分比.
%nice: nice操作所使用的CPU的百分比
.%sys: 在系统级别(kernel)运行所使用CPU的百分比.
%iowait: CPU等待硬件I/O时,所占用CPU百分比.
%idle: CPU空闲时间的百分比.
Device段:
tps: 每秒钟发送到的I/O请求数.
Blk_read /s: 每秒读取的block数.
Blk_wrtn/s: 每秒写入的block数.
Blk_read:   读入的block总数.
Blk_wrtn:  写入的block总数.
*
*
*/
