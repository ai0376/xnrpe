#include "xcommon.h"
#include "cJSON.h"

command_task commands_array[MAX_COMMAND_NUM];
char pid_file_path[MAX_INPUT_BUFFER]={0};
char *log_file=NULL ;

int command_array_size= 0 ;

int setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if(opts < 0)
    {
        return ERROR;
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0)
    {
        return ERROR;
    }
    else
    {
        return OK;
    }
}

int resolve_host_name(char* hostname, struct in_addr* addr)
{
    struct addrinfo *res;

    int result = getaddrinfo (hostname, NULL, NULL, &res);
    if (result == 0)
    {
        memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr, sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
}

int start_connect(char *host, int port)
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (resolve_host_name(host, &(address.sin_addr)) != 0)
    {
        inet_pton(PF_INET, host, &(address.sin_addr));
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket() failed");
        return ERROR;
    }
    if(connect(fd, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
        perror("connect() failed");
        close(fd);
        return ERROR;
    }
    return fd;
}

int report_tcp_information(char *info, int len, int recv_flag)
{
    struct sockaddr_in address;
    struct timeval timeout={sock_send_recv_timeout,0};;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
#ifdef _XNRPE_DEBUG
    //fprintf(stdout,"\nremote:%s:%d\n",server_address,server_port);
    logit(LL_DEBUG, "remote:%s:%d",server_address,server_port);
#endif
    address.sin_port = htons(server_port);

    if (resolve_host_name(server_address, &(address.sin_addr)) != 0)
    {
        inet_pton(PF_INET, server_address, &(address.sin_addr));
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        //perror("socket() failed");
        logit(LL_WARNING, "socket() failed");
        return ERROR;
    }
    if(connect(fd, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
        //perror("connect() failed");
        logit(LL_WARNING, "connect() failed");
        close(fd);
        return ERROR;
    }

    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    int ret = 0;
    ret = send_tcp_all(fd , info, len);
    if(recv_flag)
    {
        ret = read_response(fd, info);
        if(ret > 0)
            handle_response_message(info, ret);
    }
    close(fd);
    return ret;
}

int send_tcp_all(int s,char *buf, int len)
{
    int total = 0;
    int bytesleft = len;
    int n = 0;

    /* send all the data */
    while (total < len)
    {
        n = send(s, buf + total, bytesleft, 0); /* send some data */
        if (n == -1)            /* break on error */
            break;
        /* apply bytes we sent */
        total += n;
        bytesleft -= n;
    }
#ifdef _XNRPE_DEBUG
    //fprintf(stdout,"\n\nsend_tcp_all:%d----send total=%d\n\n",len,total);
    logit(LL_DEBUG, "send_tcp_all:%d----send total=%d",len,total);
#endif
    len = total;                /* return number of bytes actually sent here */
    return n == -1 ? -1 : 0;    /* return -1 on failure, 0 on success */
}

int read_response(int sock, char *buf)
{
    int common_size,total,json_size;
    total = 0;
    common_size = MESSAGE_HEAD_LEN;

    int recvd = recv(sock, buf, MAX_INPUT_BUFFER, 0);
    if(recvd == -1 && errno == EAGAIN)
    {
        //fprintf(stdout, "recv timeout\n");
        logit(LL_WARNING, "recv timeout");
        return -1;
    }
    else if(recvd <= 0)
    {
        //fprintf(stdout, "recv failed\n");
        logit(LL_WARNING, "recv failed");
        return -1;
    }
    total += recvd;
    json_size =((buf[2]&0xFF)<<24|(buf[3]&0xFF)<<16|(buf[4]&0xFF)<<8|(buf[5]&0xFF)<<0);
    common_size = json_size+MESSAGE_HEAD_LEN;
    while(total < common_size)
    {
        recvd = recv(sock, buf+total,MAX_INPUT_BUFFER, 0);
        if(recvd == -1 && errno == EAGAIN)
        {
            //fprintf(stdout, "recv timeout\n");
            logit(LL_WARNING, "recv timeout");
            return -1;
        }
        else if(recvd <= 0)
        {
            //fprintf(stdout, "recv failed\n");
            logit(LL_WARNING, "recv failed");
            return -1;
        }
        total += recvd;
    }
    return total;
}

/*
type: 0,heartbeat; 1 report message
*/
int pack_msg(char *inbuf, unsigned int len, char *outbuf,char type)
{
    char head[MESSAGE_HEAD_LEN]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    head[2]=0x000000ff&(len >> 24);
    head[3]=0x000000ff&(len >> 16);
    head[4]=0x000000ff&(len >> 8);
    head[5]=0x000000ff&(len >> 0);
    head[6]=0xff&type;
    memcpy((void *)outbuf,(void *)&head, MESSAGE_HEAD_LEN);
    memcpy((void *)(outbuf+MESSAGE_HEAD_LEN), (void *)inbuf, len);
    return (MESSAGE_HEAD_LEN+len);
}

/*
json example: [{"command":"check_disk","id":"001"}]
*/
int handle_heartbeat_respon_msg(char *str)
{
    int i;

    cJSON *root =cJSON_Parse(str);
    if(root == NULL)
    {
        logit(LL_WARNING, "Json Parse error");
        return -1;
    }
    if(!cJSON_IsArray(root))
    {
        logit(LL_WARNING, "item not array");
        cJSON_Delete(root);
        return -1;
    }
    memset((char *)commands_array, 0, sizeof(command_task) * MAX_COMMAND_NUM);
    command_array_size = 0;

    for(i = 0 ; i < cJSON_GetArraySize(root); i++)
    {
        cJSON *obj = cJSON_GetArrayItem(root, i);
        if(!(cJSON_HasObjectItem(obj,"neId"))||!(cJSON_HasObjectItem(obj,"neType")))
        {
            continue;
        }
        cJSON* neId_obj = cJSON_GetObjectItem(obj,"neId");
        cJSON* neType_obj = cJSON_GetObjectItem(obj,"neType");
        if((neId_obj->valuestring != NULL ) && (neType_obj->valuestring != NULL))
        {
            // add cmd_name & id  in list
            command_task cmd;
            strcpy(cmd.neType,neType_obj->valuestring);
            strcpy(cmd.neId,neId_obj->valuestring);
            commands_array[i] = cmd;
            command_array_size ++;
        }
    }
#ifdef _XNRPE_DEBUG
    //printf("cmd-array-size: %d\n",command_array_size);
    logit(LL_DEBUG, "cmd-array-size: %d",command_array_size);
    char *p = cJSON_PrintUnformatted(root);
    logit(LL_DEBUG, "response:%s",p);
    if(p != NULL)
    {
        free(p);
        p = NULL;
    }
#endif // _XNRPE_DEBUG
    cJSON_Delete(root);
    return 0;
}

int handle_response_message(char *buf, int len)
{
    char *msg = buf;
    char temp[MAX_INPUT_BUFFER]="";
    int json_size = ((buf[2]&0xFF)<<24|(buf[3]&0xFF)<<16|(buf[4]&0xFF)<<8|(buf[5]&0xFF)<<0);
#ifdef _XNRPE_DEBUG
             logit(LL_DEBUG, "len = %d, json_size=%d", len,json_size);
#endif
    memcpy(temp, msg+MESSAGE_HEAD_LEN, json_size);
    char type = buf[6];
    if(type == 0)
    {
        handle_heartbeat_respon_msg(temp);
    }
    else
    {

    }
    return 0;
}

int my_system(char *command, char *outbuf)
{
    FILE *fp = NULL;
    int size = 0;
    int byte_read = 0;
    char *buffer=outbuf;
    char tempbuff[MAX_SYSTEM_RETRUN_BUFFER]={0};
#ifdef _XNRPE_DEBUG
    //printf("%s\n",command);
    logit(LL_DEBUG, "%s\n",command);
#endif
    if((fp=popen(command, "r")) == NULL)
    {
        //fprintf(stdout,"popen error: %s",strerror(errno));
        logit(LL_WARNING, "popen error: %s",strerror(errno));
        return -1;
    }
    
    while((byte_read = fread(tempbuff+size , 1, sizeof(tempbuff),fp)) > 0)
    {
        size += byte_read;
    }
    pclose(fp);
    fp = NULL;
    if(size != 0)
    {
        strncpy(buffer, tempbuff, size-1);
        return size-1;
    }
    else
        return 0;  
}

void create_pid_file()
{
    if(strcmp(pid_file_path,"\0") == 0)    
    {
        strcpy(pid_file_path, CONFIG_DEFAULT_PID_FILE);
    }
    FILE *fp = fopen(pid_file_path, "w");
    if(fp)
    {
        fprintf(fp,"%d\n",(int)getpid());
        logit(LL_NOTICE,"create pid file success");
        fclose(fp);
    }
}

void create_daemonize()
{
    int fd;
    if(fork() != 0)
    {
        exit(0);
    }
    setsid();
    logit(LL_NOTICE,"setsid success");
    if((fd == open("/dev/null",O_RDWR, 0)) != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) 
            close(fd);
    }
}

void logit_raw(int level, const char *msg)
{
    // const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmode = (level & LL_RAW);
    //int log_to_stdout = log_file[0] == '\0';
    FILE *log_to_stdout=NULL;
    if(log_file==NULL || log_file[0] == '\0')
    {
        log_to_stdout = stdout;
    }

    level &= 0xff; /* clear flags */
    
    fp = log_to_stdout ? stdout : fopen(log_file,"a");
    if (!fp) 
        return;

    if (rawmode) 
    {
        fprintf(fp,"%s",msg);
    } 
    else 
    {
        int off;
        struct timeval tv;
        
        gettimeofday(&tv,NULL);
        off = strftime(buf,sizeof(buf),"%d %b %H:%M:%S.",localtime(&tv.tv_sec));
        snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
        
        fprintf(fp,"%d: %s %c %s\n",
            (int)getpid(), buf,c[level],msg);
    }
    fflush(fp);

    if (!log_to_stdout) fclose(fp);
   // if (server.syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

/* Like logit_raw() but with printf-alike support. This is the function that
 * is used across the code. The raw version is only used in order to dump
 * the INFO output on crash. */
void logit(int level, const char *fmt, ...)
{
    va_list ap;
    char msg[LOG_MAX_LEN];

    //if ((level&0xff) < server.verbosity) return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    logit_raw(level,msg);
}

