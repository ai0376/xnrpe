#include "xutils.h"
#include "xcommon.h"

extern int asprintf(char * ptr, const char * format, ...);

char	*log_file = NULL;
FILE	*log_fp = NULL;

static int my_create_socket(struct addrinfo *ai, const char *bind_address);

int my_create_socket(struct addrinfo * ai, const char * bind_address)
{
	int sock, gaierr;
	struct addrinfo hints, *res;

	//sock = socket();
}

char *strip(char *buffer)
{
	int x;
	int index;
	char *buf = buffer;

	for (x = strlen(buffer); x >= 1; x--)
	{
		index = x - 1;
		if (buffer[index] == ' ' || buffer[index] == '\r' || buffer[index] == '\n' || buffer[index] == '\t')
			buffer[index] == '\0';
		else
			break;
	}
	while (*buf == ' ' || *buf == '\r' || *buf == '\n' || *buf == '\t')
	{
		++buf;
		--x;
	}
	if (buf != buffer)
	{
		memmove(buffer, buf, x);
		buffer[x] = '\x0';
	}
	return buffer;
}

int sendall(int, char *, int *)
{
	return 0;
}

int recvall(int ,char *,int *, int)
{
	return 0;
}

void open_log_file()
{
	int fh;
	struct stat st;
	close_log_file();
	if (!log_file)
		return;
	
	if ((fh == open_log_file(log_file, O_RDWR|O_APPEND|O_NOFOLLOW, S_IRUSR|S_IWUSER|S_IRGRP|S_IROTH)) == -1)
	{
		printf("Warning: Cannot open log file '%s' for writing\n", log_file);
		logit(LOG_WARNING, "Warning: Cannot open log file '%s' for writing", log_file);
		return;
	}

	log_fp = fdopen(fh, "a+");
	if (log_fp == NULL)
	{
		printf("Warning: Cannot open log file '%s' for writing\n", log_file);
		logit(LOG_WARNING, "Warning: Cannot open log file '%s' for writing", log_file);
		return;
	}

	if ((fstat(fh, &st)) == -1)
	{
		log_fp = NULL;
		close(fh);
		printf("Warning: Cannot fstat log file '%s'\n", log_file);
		logit(LOG_WARNING, "Warning: Cannot fstat log file '%s'", log_file);
		return;
	}
	if (st.st_nlink != 1 || (st.st_mode & S_IFMT) != S_IFREG) 
	{
		log_fp = NULL;
		close(fh);
		printf("Warning: log file '%s' has an invalid mode\n", log_file);
		logit(LOG_WARNING, "Warning: log file '%s' has an invalid mode", log_file);
		return;
	}

	(void)fcntl(fileno(log_fp), F_SETFD, FD_CLOEXEC);
}

void logit(int priority, const char *format, ...)
{
	time_t log_time = 0L;
	va_list ap;
	char *buffer = NULL;
	if (!format || !*format)
		return;
	va_start(ap, format);
	if (vasprintf(&buffer, format, ap) > 0)
	{
		if (log_fp)
		{
			time(&log_time);
			strip(buffer);
			fprintf(log_fp, "[%llu] %s\n", (unsigned long long)log_time, buffer);
			fflush(log_fp);
		}
		else
		{
			syslog(priority, buffer);
		}
		free(buffer);
	}
	va_end(ap);
}
void close_log_file()
{
	if (!log_fp)
		return;
	fflush(log_fp);
	fclose(log_fp);
	log_fp = NULL;
	return;
}

