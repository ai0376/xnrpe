#include "xnrpe.h"
#include "xcommon.h"
#include "xutils.h"
#include "xconfig.h"

extern int asprintf(char *ptr, const char *format, ...);

#define MAX_LISTEN_SOCKS	16
#define DEFAULT_LISTEN_QUEUE_SIZE	5

extern int errno;

struct	addrinfo *listen_addrs = NULL;
int     num_listen_socks = 0;
int		listen_socks[MAX_LISTEN_SOCKS];
int server_port = DEFAULT_SERVER_PORT;

int socket_timeout = DEFAULT_SCOKET_TIMEOUT;
int address_family = AF_UNSPEC;
int num_listen_socks = 0;
int connection_timeout = DEFAULT_CONNECTION_TIMEOUT;
int       sigrestart = FALSE;
int       sigshutdown = FALSE;



void create_listener(struct addrinfo *ai)
{
	int ret;
	char      ntop[NI_MAXHOST], strport[NI_MAXSERV];
	int       listen_sock;
	int       flag = 1;

	if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
		return;

	if (num_listen_socks >= MAX_LISTEN_SOCKS) 
	{
		fprintf(stdout, "Too many listen sockets. Enlarge MAX_LISTEN_SOCKS");
		exit(1);
	}

	if ((ret = getnameinfo(ai->ai_addr, ai->ai_addrlen, ntop, sizeof(ntop),
						   strport, sizeof(strport), NI_NUMERICHOST | NI_NUMERICSERV)) != 0) 
	{
		fprintf(stdout, "getnameinfo failed");
		return;
	}
						   

	/* Create socket for listening. */
	listen_sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (listen_sock < 0) 
	{
		/* kernel may not support ipv6 */
		fprintf(stdout, "socket: %.100s", strerror(errno));
		return;
	}

	/* socket should be non-blocking */
	fcntl(listen_sock, F_SETFL, O_NONBLOCK);

	/* set the reuse address flag so we don't get errors when restarting */
	if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) 
	{
		fprintf(stdout, "setsockopt SO_REUSEADDR: %s", strerror(errno));
		return;
	}

	/* Bind the socket to the desired port. */
	if (bind(listen_sock, ai->ai_addr, ai->ai_addrlen) < 0) 
	{
		fprintf(stdout, "Bind to port %s on %s failed",strport,strerror(errno));
		close(listen_sock);
		return;
	}

	listen_socks[num_listen_socks] = listen_sock;
	num_listen_socks ++;

	/* Start listening on the port. */
	if (listen(listen_sock, listen_queue_size) < 0) 
	{
		fprintf(stdout,"listen on [%s]:%s: %.100s", ntop, strport, strerror(errno));
		exit(1);
	}

	fprintf(stdout, "Server listening on %s port %s.", ntop, strport);
	
	return ;
}

void wait_for_connections(void)
{
	struct sockaddr from;
	socklen_t fromlen;
	fd_set *fdset = NULL;
	int maxfd = 0,new_sd = 0, i, rc, retval;

	setup_wait_conn();
	while(1)
	{
		if (sigrestart == TRUE || sigshutdown == TRUE)
			break;
		for (i = 0; i < num_listen_socks; i++)
		{
			if(listen_socks[i] > maxfd)
				maxfd = listen_socks[i];
		}
		if (fdset != NULL)
			free(fdset);
		for (i = 0; i < num_listen_socks; i++)
		{
			FD_SET(listen_socks[i], fdset);
		}
		retval = select(maxfd+1, fdset, NULL, NULL, NULL);
		if (sigrestart == TURE || sigshutdown == TRUE)
			break;
		if(retval < 0)
			continue;
		for (i = 0; i < num_listen_socks ; i++)
		{
			if (!FD_ISSET(listen_socks[i], fdset))
				continue;
			fromlen = (socklen_t)sizeof(from);

			new_fd = accept(listen_socks[i], (struct sockaddr *)&from, &fromlen);
			if(new_sd < 0)
			{
				if (sigrestart == TRUE || sigshutdown == TRUE)
					break;
				if (errno == EWOULDBLOCK ||  errno == EINTR)
					continue;
				if (errno == EAGAIN)
					continue;
				if (errno == ENOBUFS)
					continue;
				break;
			}
		}

		rc = wait_conn_fork(new_sd);
		if (rc == TRUE)
			continue;
		conn_check_peer(new_sd);
		handle_connection(new_sd);
		close(new_sd);
		exit(STATE_OK);
	}
	return ;
}

void setup_wait_conn(void)
{
	return ;
}

void conn_check_peer(int sock)
{
	return ;
}

void handle_connection(int sock)
{
	return ;
}
void init_handle_conn(void)
{
	return ;
}


int main(int argc,char **argv)
{
	int result = OK;
	int x;
	unsigned int y;

}

