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
                    sprintf(json_obj,"{\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\",\"id\":\"%s\"}", disk_avgnum,array_data[11],disk_iobytes,array_data[9],array_data[0],progname,id);
                }
                else
                {
                    sprintf(json_obj,"%s,{\"PF_SERVER_DISK_AVGNUM\":\"%s\",\"PF_SERVER_DISK_BUSYRATE\":\"%s\",\"PF_SERVER_DISK_IOBYTES\":\"%s\",\"PF_SERVER_DISK_WAITTIME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\",\"PF_SERVER_DISK_NAME\":\"%s\",\"id\":\"%s\"}",json_obj,disk_avgnum,array_data[11],disk_iobytes,array_data[9],array_data[0],progname,id);
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

/*Device:         rrqm/s   wrqm/s   r/s   w/s   rsec/s   wsec/s avgrq-sz avgqu-sz   await svctm %util
sda               0.00     3.50 0.40 2.50     5.60    48.00    18.48     0.00    0.97   0.97   0.28
sdb               0.00     0.00 0.00 0.00     0.00     0.00     0.00     0.00    0.00   0.00   0.00
sdc               0.00     0.00 0.00 0.00     0.00     0.00     0.00     0.00    0.00   0.00   0.00
sdd               0.00     0.00 0.00 0.00     0.00     0.00     0.00     0.00    0.00   0.00   0.00
sde               0.00     0.10 0.30 0.20     2.40     2.40     9.60     0.00    1.60   1.60   0.08
sdf              17.40     0.50 102.00 0.20 12095.20     5.60   118.40     0.70    6.81   2.09 21.36
sdg             232.40     1.90 379.70 0.50 76451.20    19.20   201.13     4.94   13.78   2.45 93.16

rrqm/s:   ÿ����� merge �Ķ�������Ŀ���� delta(rmerge)/s
wrqm/s: ÿ����� merge ��д������Ŀ���� delta(wmerge)/s
r/s:           ÿ����ɵĶ� I/O �豸�������� delta(rio)/s
w/s:         ÿ����ɵ�д I/O �豸�������� delta(wio)/s
rsec/s:    ÿ������������� delta(rsect)/s
wsec/s: ÿ��д���������� delta(wsect)/s
rkB/s:     ÿ���K�ֽ������� rsect/s ��һ�룬��Ϊÿ������СΪ512�ֽڡ�(��Ҫ����)
wkB/s:    ÿ��дK�ֽ������� wsect/s ��һ�롣(��Ҫ����)
avgrq-sz:ƽ��ÿ���豸I/O���������ݴ�С (����)��delta(rsect+wsect)/delta(rio+wio)
avgqu-sz:ƽ��I/O���г��ȡ��� delta(aveq)/s/1000 (��Ϊaveq�ĵ�λΪ����)��
await:    ƽ��ÿ���豸I/O�����ĵȴ�ʱ�� (����)���� delta(ruse+wuse)/delta(rio+wio)
svctm: ƽ��ÿ���豸I/O�����ķ���ʱ�� (����)���� delta(use)/delta(rio+wio)
%util:     һ�����аٷ�֮���ٵ�ʱ������ I/O ����������˵һ�����ж���ʱ�� I/O �����Ƿǿյġ��� delta(use)/s/1000 (��Ϊuse�ĵ�λΪ����)

��� %util �ӽ� 100%��˵��������I/O����̫�࣬I/Oϵͳ�Ѿ������ɣ��ô���
���ܴ���ƿ����
idleС��70% IOѹ���ͽϴ���,һ���ȡ�ٶ��н϶��wait.
ͬʱ���Խ��vmstat �鿴�鿴b����(�ȴ���Դ�Ľ�����)��wa����(IO�ȴ���ռ�õ�CPUʱ��İٷֱ�,�߹�30%ʱIOѹ����)

avg-cpu��:
%user: ���û�����������ʹ�õ�CPU�İٷֱ�.
%nice: nice������ʹ�õ�CPU�İٷֱ�
.%sys: ��ϵͳ����(kernel)������ʹ��CPU�İٷֱ�.
%iowait: CPU�ȴ�Ӳ��I/Oʱ,��ռ��CPU�ٷֱ�.
%idle: CPU����ʱ��İٷֱ�.
Device��:
tps: ÿ���ӷ��͵���I/O������.
Blk_read /s: ÿ���ȡ��block��.
Blk_wrtn/s: ÿ��д���block��.
Blk_read:   �����block����.
Blk_wrtn:  д���block����.
*
*
*/
