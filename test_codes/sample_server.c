#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#define BUFFER_SIZE 2048
#define MAX_EVENTS 10

int main(int argc, char * argv[])
{
    int server_sockfd;
    int client_sockfd;
    int len,port=5566;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    int sin_size;
    char buf[BUFFER_SIZE];
    if(argc < 2)
    {
        fprintf(stdout, "Please input: ./exe port");
        exit(-1);
    }
    port = atoi(argv[1]);
    fprintf(stdout, "\n\tListening port:%d\n\n",port);
    memset(&my_addr,0,sizeof(my_addr));
    my_addr.sin_family=AF_INET;
    my_addr.sin_addr.s_addr=INADDR_ANY;
    my_addr.sin_port=htons(port);

    if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
        perror("socket");
        return 1;
    }

    if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
    {
        perror("bind");
        return 1;
    }

    listen(server_sockfd,5);
    sin_size=sizeof(struct sockaddr_in);

	int epoll_fd;
	epoll_fd=epoll_create(MAX_EVENTS);
	if(epoll_fd==-1)
	{
		perror("epoll_create failed");
		exit(EXIT_FAILURE);
	}
   	struct epoll_event ev;

	ev.events=EPOLLIN|EPOLLET;
	ev.data.fd=server_sockfd;

	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,server_sockfd,&ev)==-1)
	{
		perror("epll_ctl:server_sockfd register failed");
		exit(EXIT_FAILURE);
	}
	int nfds;
	int count = 0;
    while(1)
	{
		struct epoll_event events[MAX_EVENTS];
		nfds=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
		if(nfds==-1)
		{
			perror("start epoll_wait failed");
			exit(EXIT_FAILURE);
		}
		int i;
		for(i=0;i<nfds;i++)
		{

			if(events[i].data.fd==server_sockfd)
			{

        		if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
				{
					perror("accept client_sockfd failed");
					exit(EXIT_FAILURE);
				}
				count++;
				ev.events=EPOLLIN;
				ev.data.fd=client_sockfd;
				if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_sockfd,&ev)==-1)
				{
					perror("epoll_ctl:client_sockfd register failed");
					exit(EXIT_FAILURE);
				}
				printf("\n\naccept client %s\n",inet_ntoa(remote_addr.sin_addr));
				printf("\ncount:%d\n",count);
			}
			else if(events[i].events & EPOLLIN)
			{
				/*if((events[i].data.fd) <= 0)
					continue;*/
				bzero(buf,BUFFER_SIZE);
				len=recv(events[i].data.fd,buf,BUFFER_SIZE,0);//read(events[i].data.fd , buf , sizeof(buf));
				if(len <= 0)
				{
					struct epoll_event event_del;
					event_del.data.fd = events[i].data.fd;
					event_del.events = 0;
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_del.data.fd, &event_del);
					printf("\n\nxxx---%d\n",count);
					close(events[i].data.fd);
				}
				else
				{
					char type = buf[6];
                    if(type == 0)
                    {
                        int ret = send(events[i].data.fd,buf,len,0);
                        printf("send: %d\n", ret);
                    }
                   /* else if(type == 1)
                    {
                        char tmp[100]={0};
                        char success[20]={"{\"result\":0}"};
                        int suc_len = strlen(success);
                        char header[7]={0x0,0x0,0x0,0x0,0x0,0x0,0x0};
                        header[2]=0x000000ff&(suc_len >> 24);
                        header[3]=0x000000ff&(suc_len >> 16);
                        header[4]=0x000000ff&(suc_len >> 8);
                        header[5]=0x000000ff&(suc_len >> 0);
                        header[6]=0xff&type;
                        memcpy(tmp,header,7);
                        memcpy(tmp+7,success,suc_len);
                        suc_len = suc_len+7;
                        int ret = send(events[i].data.fd,tmp,suc_len,0);
                        printf("send: %d\n", ret);
                    }*/
				}
				printf("receive from client:%d-----%d\n\n",len,count);
			}
			else
			{
				printf("\nHELLO\n");
			}
			/*else
			{
				len=recv(client_sockfd,buf,BUFFER_SIZE,0);
				if(len<0)
				{
					perror("receive from client failed");
					exit(EXIT_FAILURE);
				}
				printf("receive from client:%d",len);
				//send(client_sockfd,"I have received your message.",30,0);
			}*/
		}
	}
	return 0;
}
