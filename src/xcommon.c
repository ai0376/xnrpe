#include "xcommon.h"

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
    if (result == 0) {
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
    fprintf(stdout,"\nremote:%s:%d\n",server_address,server_port);
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
    if(recv_flag)
    {
        ret = send_tcp_and_recv(fd, info, len);
    }
    else
    {
        ret = send_tcp_all(fd , info, len);
    }
    close(fd);
    return ret;
}

int send_tcp_and_recv(int s, char *buf, int len)
{
    int total = 0;
	int bytesleft = len;
	int n = 0;

	/* send all the data */
	while (total < len) {
		n = send(s, buf + total, bytesleft, 0);	/* send some data */
		if (n == -1)			/* break on error */
			break;
		/* apply bytes we sent */
		total += n;
		bytesleft -= n;
	}
    printf("\n\nsend_tcp_all:%d----send total=%d\n\n",len,total);
	len = total;

	char recv_buf[MAX_INPUT_BUFFER]="";
	bzero(recv_buf, MAX_INPUT_BUFFER);
    int recvd = recv(s, recv_buf,MAX_INPUT_BUFFER, 0);
    if(recvd == -1 &&errno == EAGAIN)
    {
        fprintf(stdout, "\nrecv timeout\n");
    }
    return recvd;
}

int send_tcp_all(int s,char *buf, int len)
{
    int total = 0;
	int bytesleft = len;
	int n = 0;

	/* send all the data */
	while (total < len) {
		n = send(s, buf + total, bytesleft, 0);	/* send some data */
		if (n == -1)			/* break on error */
			break;
		/* apply bytes we sent */
		total += n;
		bytesleft -= n;
	}
    printf("\n\nsend_tcp_all:%d----send total=%d\n\n",len,total);
	len = total;				/* return number of bytes actually sent here */
	return n == -1 ? -1 : 0;	/* return -1 on failure, 0 on success */
}

int pack_msg(char *inbuf, unsigned int len, char *outbuf)
{
    char head[6]={0x00,0x00,0x00,0x00,0x00,0x00};
	head[2]=0x000000ff&(len >> 24);
    head[3]=0x000000ff&(len >> 16);
    head[4]=0x000000ff&(len >> 8);
    head[5]=0x000000ff&(len >> 0);
    memcpy((void *)outbuf,(void *)&head, 6);
    memcpy((void *)(outbuf+6), (void *)inbuf, len);
	return (6+len);
}


