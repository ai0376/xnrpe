#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define  BUF_SIZE  20480
#define BUFFER_SIZE 819200
#define ID_SIZE 50

#define IN_RUNSTAT_SHELL ". ~/.bash_profile;onstat -p | sed -n '/^[0-9]/p' | awk '{ printf \"%s \", $0 }'"

char json_obj[BUFFER_SIZE]={0};
char json_array[BUFFER_SIZE]={0};
char array_data[100][1024];
char buf[BUFFER_SIZE]={0};

char id[ID_SIZE]={0};
const char *netType = "PF-DB-INFORMIX-RUNINFO";
const char *neTopType ="PF-DB-INFORMIX";
char *neName="RUNINFO";

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
    int val1, val2,val3;
    char lock_waits_str[50]={0};
    char buffer_waits_str[50]={0};

    ret = process_arguments(argc,argv);
    if(ret != 0)
    {
        return -1;
    }
    fp = popen(IN_RUNSTAT_SHELL , "r");
    if(fp == NULL)
    {
        sprintf(buf,"error:%s\n",strerror(errno));
        fprintf(stdout,"%s\n",buf);
        return -1;
    }
    
    while(fgets(buf, BUF_SIZE,fp)!= NULL)
    {
        memset(array_data, 0, 100 * 1024);
        memset(lock_waits_str, 0, 50);
        memset(buffer_waits_str, 0, 50);
        ret = split(buf, seg, array_data);
        sscanf(array_data[31], "%d", &val1);
        sscanf(array_data[2], "%d", &val2);
        sscanf(array_data[6], "%d", &val3);
        sprintf(buffer_waits_str, "%.2f", (float)val1/val2 + val3);
        sscanf(array_data[32], "%d", &val1);
        sscanf(array_data[33], "%d", &val2);
        sprintf(lock_waits_str, "%.2f", (float)val1/val2);
        if(line == 0)
        {
            sprintf(json_obj,"{\"values\":{\"PF_DB_RUNINFO_READNUM\":\"%s\",\"PF_DB_RUNINFO_PAGENUM\":\"%s\",\"PF_DB_RUNINFO_BUFFERNUM\":\"%s\",\"PF_DB_RUNINFO_CASCADERATE\":\"%s\",\"PF_DB_RUNINFO_WRITENUM\":\"%s\",\"PF_DB_RUNINFO_PAGEWRITENUM\":\"%s\",\"PF_DB_RUNINFO_BUFFERWRITENUM\":\"%s\",\"PF_DB_RUNINFO_CASCADEWRITERATE\":\"%s\",\"PF_DB_RUNINFO_ISAMTOT\":\"%s\",\"PF_DB_RUNINFO_OPEN\":\"%s\",\"PF_DB_RUNINFO_START\":\"%s\",\"PF_DB_RUNINFO_READ\":\"%s\",\"PF_DB_RUNINFO_WRITE\":\"%s\",\"PF_DB_RUNINFO_REWRITE\":\"%s\",\"PF_DB_RUNINFO_DELETE\":\"%s\",\"PF_DB_RUNINFO_COMMIT\":\"%s\",\"PF_DB_RUNINFO_ROLLBACKK\":\"%s\",\"PF_DB_RUNINFO_OVERLOCK\":\"%s\",\"PF_DB_RUNINFO_OVERBUFFER\":\"%s\",\"PF_DB_RUNINFO_USERCPU\":\"%s\",\"PF_DB_RUNINFO_SYSCPU\":\"%s\",\"PF_DB_RUNINFO_BUFFERWAIT\":\"%s\",\"PF_DB_RUNINFO_LOCKWAIT\":\"%s\",\"PF_DB_RUNINFO_LOCKREQ\":\"%s\",\"PF_DB_RUNINFO_DEADLOCK\":\"%s\",\"PF_DB_RUNINFO_TIMEOUTDEADLOCK\":\"%s\",\"PF_DB_RUNINFO_CKPWAITS\":\"%s\",\"PF_DB_RUNINFO_SEQSCANS\":\"%s\",\"PF_DB_RUNINFO_IXDARA\":\"%s\",\"PF_DB_RUNINFO_IDXRA\":\"%s\",\"PF_DB_RUNINFO_DARA\":\"%s\",\"PF_DB_RUNINFO_RAQGXUSED\":\"%s\",\"PF_DB_RUNINFO_LCHWAITS\":\"%s\",\"PF_DB_RUNINFO_BUFFERWAIT2\":\"%s\",\"PF_DB_RUNINFO_LOKWAITS2\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", array_data[0],array_data[1] ,array_data[2],array_data[3],array_data[4],array_data[5],array_data[6],array_data[7],array_data[8],array_data[9],array_data[10],	\
                array_data[11],array_data[12],array_data[13],array_data[14],array_data[15],array_data[16],array_data[24],array_data[26],	\
                array_data[27],array_data[28],array_data[31],array_data[32],array_data[33],array_data[34],array_data[35],array_data[36],	\
                array_data[38],array_data[39],array_data[40],array_data[41],array_data[ret-2],array_data[ret-1],buffer_waits_str,lock_waits_str,netType,neTopType,id,neName);
        }
        else
        {
            sprintf(json_obj,"%s,{\"values\":{\"PF_DB_RUNINFO_READNUM\":\"%s\",\"PF_DB_RUNINFO_PAGENUM\":\"%s\",\"PF_DB_RUNINFO_BUFFERNUM\":\"%s\",\"PF_DB_RUNINFO_CASCADERATE\":\"%s\",\"PF_DB_RUNINFO_WRITENUM\":\"%s\",\"PF_DB_RUNINFO_PAGEWRITENUM\":\"%s\",\"PF_DB_RUNINFO_BUFFERWRITENUM\":\"%s\",\"PF_DB_RUNINFO_CASCADEWRITERATE\":\"%s\",\"PF_DB_RUNINFO_ISAMTOT\":\"%s\",\"PF_DB_RUNINFO_OPEN\":\"%s\",\"PF_DB_RUNINFO_START\":\"%s\",\"PF_DB_RUNINFO_READ\":\"%s\",\"PF_DB_RUNINFO_WRITE\":\"%s\",\"PF_DB_RUNINFO_REWRITE\":\"%s\",\"PF_DB_RUNINFO_DELETE\":\"%s\",\"PF_DB_RUNINFO_COMMIT\":\"%s\",\"PF_DB_RUNINFO_ROLLBACKK\":\"%s\",\"PF_DB_RUNINFO_OVERLOCK\":\"%s\",\"PF_DB_RUNINFO_OVERBUFFER\":\"%s\",\"PF_DB_RUNINFO_USERCPU\":\"%s\",\"PF_DB_RUNINFO_SYSCPU\":\"%s\",\"PF_DB_RUNINFO_BUFFERWAIT\":\"%s\",\"PF_DB_RUNINFO_LOCKWAIT\":\"%s\",\"PF_DB_RUNINFO_LOCKREQ\":\"%s\",\"PF_DB_RUNINFO_DEADLOCK\":\"%s\",\"PF_DB_RUNINFO_TIMEOUTDEADLOCK\":\"%s\",\"PF_DB_RUNINFO_CKPWAITS\":\"%s\",\"PF_DB_RUNINFO_SEQSCANS\":\"%s\",\"PF_DB_RUNINFO_IXDARA\":\"%s\",\"PF_DB_RUNINFO_IDXRA\":\"%s\",\"PF_DB_RUNINFO_DARA\":\"%s\",\"PF_DB_RUNINFO_RAQGXUSED\":\"%s\",\"PF_DB_RUNINFO_LCHWAITS\":\"%s\",\"PF_DB_RUNINFO_BUFFERWAIT2\":\"%s\",\"PF_DB_RUNINFO_LOKWAITS2\":\"%s\"},\"neType\":\"%s\",\"neTopType\":\"%s\",\"neId\":\"%s\",\"neName\":\"%s\"}", json_obj, array_data[0],array_data[1] ,array_data[2],array_data[3],array_data[4],array_data[5],array_data[6],array_data[7],array_data[8],array_data[9],array_data[10],	\
                array_data[11],array_data[12],array_data[13],array_data[14],array_data[15],array_data[16],array_data[24],array_data[26],	\
                array_data[27],array_data[28],array_data[31],array_data[32],array_data[33],array_data[34],array_data[35],array_data[36],	\
                array_data[38],array_data[39],array_data[40],array_data[41],array_data[ret-2],array_data[ret-1],buffer_waits_str,lock_waits_str,netType,neTopType,id,neName);
        }
        line++;
    }
    printf("%s\n",json_obj);
    fclose(fp);
    fp = NULL;
    return 0;
}