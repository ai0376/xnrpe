#include "xcommon.h"
#include "cJSON.h"

command_task commands_array[MAX_COMMAND_NUM];

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
    fprintf(stdout,"\nremote:%s:%d\n",server_address,server_port);
#endif
    address.sin_port = htons(server_port);

    if (resolve_host_name(server_address, &(address.sin_addr)) != 0)
    {
        inet_pton(PF_INET, server_address, &(address.sin_addr));
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
    fprintf(stdout,"\n\nsend_tcp_all:%d----send total=%d\n\n",len,total);
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
        fprintf(stdout, "recv timeout\n");
        return -1;
    }
    else if(recvd <= 0)
    {
        fprintf(stdout, "recv failed\n");
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
            fprintf(stdout, "recv timeout\n");
            return -1;
        }
        else if(recvd <= 0)
        {
            fprintf(stdout, "recv failed\n");
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
        fprintf(stdout, "Json Parse error");
        return -1;
    }
    if(!cJSON_IsArray(root))
    {
        fprintf(stdout, "item not array");
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
    printf("cmd-array-size: %d\n",command_array_size);
   
    char *p = cJSON_PrintUnformatted(root);
    printf("response:%s\n", p);
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
    printf("\nlen = %d, json_size=%d\n", len,json_size);
    memcpy(temp, msg+MESSAGE_HEAD_LEN, json_size);
    //printf("json: %s\n",temp);
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
    printf("%s\n",command);
#endif
    if((fp=popen(command, "r")) == NULL)
    {
        fprintf(stdout,"popen error: %s",strerror(errno));
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


