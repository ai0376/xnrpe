#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
 
#define MAX_LINE 1024
#define INET_ADDR_STR 16 
 

int main(int argc,char **argv)
{
    struct sockaddr_in sin;     //������ͨ�ŵ�ַ�ṹ
    struct sockaddr_in cin;     //����ͻ���ͨ�ŵ�ַ�ṹ
    int l_fd;
    int c_fd;
    socklen_t len;
    char buf[MAX_LINE];     //�洢�������ݵĻ�����
    char addr_p[INET_ADDR_STR]; //�洢�ͻ��˵�ַ�Ļ�����
    int port = 5566;
    int n;
    bzero((void *)&sin,sizeof(sin));
    sin.sin_family = AF_INET;   //ʹ��IPV4ͨ����
    sin.sin_addr.s_addr = INADDR_ANY;   //���������Խ��������ַ
    sin.sin_port = htons(port); //�˿�ת��Ϊ�����ֽ���
     
    l_fd = socket(AF_INET,SOCK_STREAM,0);   //�����׽���,ʹ��TCPЭ��
    bind(l_fd,(struct sockaddr *)&sin,sizeof(sin));
     
    listen(l_fd,10);    //��ʼ��������
     
    printf("waiting ....\n");
    while(1)
    {
        c_fd = accept(l_fd,(struct sockaddr *)&cin,&len);
         
        n = read(c_fd,buf,MAX_LINE);    //��ȡ�ͻ��˷���������Ϣ
        inet_ntop(AF_INET,&cin.sin_addr,addr_p,INET_ADDR_STR);      //���ͻ��˴�����ַת��Ϊ�ַ���
        printf("client IP is %s,port is %d\n",addr_p,ntohs(cin.sin_port));
        printf("content is : %s\n", buf);   //��ӡ�ͻ��˷��͹���������
		printf("len is : %d\n", n);   //��ӡ�ͻ��˷��͹���������
      
        //close(c_fd);
    }
    printf("buf = %s\n",buf);
    if((close(l_fd)) == -1)
    {
        perror("fail to close\n");
        exit(1);
    }
    return 0;
}