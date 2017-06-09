#include "xcommon.h"



command  *command_list = NULL;
char server_address[NI_MAXHOST]="";
char local_host[NI_MAXHOST]="127.0.0.1";
int server_port=DEFAULT_SERVER_PORT;
int sock_send_recv_timeout = DEFAULT_SOCK_SEND_RECV_TIMEOUT;
int heartbeat_time=DEFAULT_HEARTBEAT_TIME;
int report_time = DEFAULT_HEARTBEAT_TIME;

extern int errno;
struct epoll_event event;
struct epoll_event ev[EPOLL_LEN];
char  config_file[MAX_INPUT_BUFFER] = "nrpe.cfg";

int listen_socks[MAX_LISTEN_SOCKS];
int num_listen_socks = 0;
int socket_timeout = DEFAULT_SOCKET_TIMEOUT;
int connection_timeout = DEFAULT_CONNECTION_TIMEOUT;
int task_num = 2;
int task_flag = true;
int report_flag = false;
int show_help = false;

//signal function to exit process
int signal_flag_int = true;
int signal_flag_term = true;
void fun_int(int arg)
{
    signal_flag_int = false;
}
void fun_term(int arg)
{
    signal_flag_term = false;
}

void handle_pipe(int sig)
{
    fprintf(stdout, "catch a SIGPIPE signal");
    return;
}

void *report_task(void *args)
{
    int index=0;
    while(true)
    {
        //if(report_flag)
        {
            if(command_array_size > 0)
            {
                char cmd[512]={0};
                char id[512]={0};
                char buf[MAX_SYSTEM_RETRUN_BUFFER]={0};
                char json_array[MAX_SYSTEM_RETRUN_BUFFER]={0};
                char outbuf[MAX_SYSTEM_RETRUN_BUFFER]={0};
                char mycmd[MAX_COMMAND_NUM]={0};
                int len = 0;
                for(index = 0; index < command_array_size ; index++)
                {
                    memset(cmd,0, 512);
                    memset(id,0, 512);
                    strcpy(cmd,commands_array[index].neType);
                    strcpy(id,commands_array[index].neId);
                    //command *tempcommand = find_command(cmd);
                    /*if(tempcommand == NULL)
                    {
                        fprintf(stdout, "not support cmd:%s\n",cmd);
                        continue;
                    }*/
                    if(strcmp(cmd, "PF-SERVER-UNIX")==0)
                    {
                        command *tempcommand = command_list;
                        bzero(outbuf,MAX_SYSTEM_RETRUN_BUFFER);
                        bzero(json_array, MAX_SYSTEM_RETRUN_BUFFER);
                        int cc=0,flag=0;
                        while(tempcommand != NULL)
                        {
                            bzero(mycmd, MAX_COMMAND_NUM);
                            
                            //tempcommand->command_name
                            strcpy(mycmd, tempcommand->command_line);
                            strcat(mycmd, " ");
                            strcat(mycmd, id);
                            bzero(buf,MAX_SYSTEM_RETRUN_BUFFER);
                            len = my_system(mycmd,buf);
                            if(len == 0)
                            {
                                tempcommand = tempcommand->next;
                                continue;
                            }
                            if(cc==0)
                                sprintf(json_array, "%s",buf);
                            else
                                sprintf(json_array, "%s,%s",json_array,buf);
                            tempcommand = tempcommand->next;
                            flag = 1;
                            cc++;
                        }
                        if(flag)
                        {
                            bzero(buf, MAX_SYSTEM_RETRUN_BUFFER);
                            sprintf(buf, "[%s]",json_array);
                            len = strlen(buf);
                            len = pack_msg(buf,len,outbuf,1);
                            report_tcp_information(outbuf, len,0);
                        }

                        printf("\n*************\n");
                        /*bzero(outbuf,MAX_INPUT_BUFFER);
                        sprintf(buf, "[{\"values\":{\"PF_SERVER_DISK_IOTIMES\":\"NA\",\"PF_SERVER_DISK_WAITTIME\":\"0.38\",\"PF_SERVER_DISK_BUSYRATE\":\"0.04\",\"PF_SERVER_DISK_IOBYTES\":\"57.73\",\"PF_SERVER_DISK_NAME\":\"cciss/c0d0p1\"},\"neId\":\"%s\",\"neTopType\":\"PF-SERVER-UNIX\",\"neType\":\"PF-SERVER-UNIX-DISKIO\",\"neName\":\"cciss/c0d0p1\"}]",id);
                        len = strlen(buf);
                        len = pack_msg(buf, len,outbuf,1);
                        report_tcp_information(outbuf, len,0);*/
#ifdef _XNRPE_DEBUG
                        fprintf(stdout,"had report [%s] info\n",cmd);
#endif // _XNRPE_DEBUG
                    }
                }
            }
        }
       // else
        {

        }
        sleep(report_time);
    }
}
//timer task and send message
void *task(void *args)
{

    ARGS *args_value = (ARGS *)args;
    int time = args_value->time;
    char cmd[MAX_INPUT_BUFFER] ="";
    char buf[MAX_INPUT_BUFFER] ="";
    char outbuf[MAX_INPUT_BUFFER]="";
    int len = 0;
    strcpy(cmd, args_value->cmd);
    while(1)
    {
        memset(outbuf,0,MAX_INPUT_BUFFER);
        //send msg
        if(!task_flag)
        {
            break;
        }
        if(strcmp(cmd, "check_disk") == 0)
        {
            fprintf(stdout,"time: %d cmd: %s\n",time,cmd);
            strcpy(buf, "[{\"values\":{\"PF_SERVER_DISK_IOTIMES\":\"NA\",\"PF_SERVER_DISK_WAITTIME\":\"0.38\",\"PF_SERVER_DISK_BUSYRATE\":\"0.04\",\"PF_SERVER_DISK_IOBYTES\":\"57.73\",\"PF_SERVER_DISK_NAME\":\"cciss/c0d0p1\"},\"neId\":\"402885ef5c103953015c104e0c920001\",\"neTopType\":\"PF-SERVER-UNIX\",\"neType\":\"PF-SERVER-UNIX-DISKIO\",\"neName\":\"cciss/c0d0p1\"}]");
            len = strlen(buf);
            len = pack_msg(buf, len,outbuf,1);
            report_tcp_information(outbuf, len, 0);
           // command *tempcommand = find_command(cmd);
           // printf("\n\n%s:%s\n\n",tempcommand->command_name,tempcommand->command_line);
        }
        else if(strcmp(cmd, "check_cpu") == 0)
        {
            /*command *tempcommand = find_command(cmd);
            printf("\n\n%s:%s\n\n",tempcommand->command_name,tempcommand->command_line);*/
            fprintf(stdout,"time: %d cmd: %s\n",time,cmd);
            strcpy(buf, "[{\"values\":{\"PF_SERVER_DISK_IOTIMES\":\"NA\",\"PF_SERVER_DISK_WAITTIME\":\"0.38\",\"PF_SERVER_DISK_BUSYRATE\":\"0.04\",\"PF_SERVER_DISK_IOBYTES\":\"57.73\",\"PF_SERVER_DISK_NAME\":\"cciss/c0d0p1\"},\"neId\":\"402885ef5c103953015c104e0c920001\",\"neTopType\":\"PF-SERVER-UNIX\",\"neType\":\"PF-SERVER-UNIX-DISKIO\",\"neName\":\"cciss/c0d0p1\"}]");
            len = strlen(buf);
            len = pack_msg(buf, len,outbuf,1);
            report_tcp_information(outbuf, len,0);
        }
        else if(strcmp(cmd, "ACK") == 0)
        {
            fprintf(stdout,"time: %d cmd: %s\n",time,cmd);
            sprintf(buf,"[{\"neIp\":\"%s\"}]",local_host);
            len = strlen(buf);
            len = pack_msg(buf, len,outbuf,0);
            report_tcp_information(outbuf, len, 1);
        }
        sleep(time);
    }
    return ;
}

void usage(int result)
{
    printf("\n");
    printf("Welcome to use XNRPE\n");
    printf("License: GPL v2 with exemptions (-h for more info)\n");

    printf("\n");
    if (result != OK || show_help == true)
    {
        printf("Usage: xnrpe -c <config_file> \n");
        printf("\n");
        printf("Options:\n");
        printf(" -c <config_file> = Name of config file to use\n");
        printf("\n");
    }
    exit(ERROR);
}
int process_arguments(int argc, char **argv)
{
    char      optchars[MAX_INPUT_BUFFER];
    int       c = 1;

    /* no options were supplied */
    if (argc < 2)
        return ERROR;

    snprintf(optchars, MAX_INPUT_BUFFER, "c:h");

    while (1)
    {
        c = getopt(argc, argv, optchars);
        if (c == -1 || c == EOF)
            break;
        /* process all arguments */
        switch (c) {

        case '?':
        case 'h':
            show_help = true;
            break;
        case 'c':
            strncpy(config_file, optarg, sizeof(config_file));
            config_file[sizeof(config_file) - 1] = '\x0';
            break;
        default:
            return ERROR;
        }
    }
    return OK;
}

int add_command(char *command_name, char *command_line)
{
    command *new_command=NULL;
    if(command_name == NULL||command_line == NULL)
    {
        return ERROR;
    }
    new_command = (command *)malloc(sizeof(command));
    if(new_command == NULL)
    {
        return ERROR;
    }
    new_command->command_name = strdup(command_name);
    if(new_command->command_name == NULL)
    {
        free(new_command);
        return ERROR;
    }
    new_command->command_line = strdup(command_line);
    if(new_command->command_line == NULL)
    {
        free(new_command->command_name);
        free(new_command);
        return ERROR;
    }
    new_command->next = command_list;
    command_list = new_command;

    return OK;
}

command *find_command(char *command_name)
{
    command *temp_command;
    for(temp_command = command_list; temp_command != NULL; temp_command=temp_command->next)
    {
        if(!strcmp(command_name, temp_command->command_name))
            return temp_command;
    }
    return NULL;
}

/* free all allocated memory */
void free_memory(void)
{
    command  *this_command;
    command  *next_command;

    /* free memory for the command list */
    this_command = command_list;
    while (this_command != NULL)
    {
        next_command = this_command->next;
        if (this_command->command_name)
            free(this_command->command_name);
        if (this_command->command_line)
            free(this_command->command_line);
        free(this_command);
        this_command = next_command;
    }

    command_list = NULL;
    return;
}

int read_config_file(char *filename)
{
   // struct stat st;
    FILE *fp;
    char config_file[MAX_FILENAME_LENGTH];
    char input_buffer[MAX_INPUT_BUFFER];
    char *input_line;
    char *temp_buffer;
    char *varname;
    char *varvalue;
    int line = 0;
    int len = 0;
    int x = 0;
    fp = fopen(filename , "r");
    if(fp == NULL)
    {
        return ERROR;
    }
    while(fgets(input_buffer, MAX_INPUT_BUFFER-1,fp))
    {
        line++;
        input_line = input_buffer;
        //skip leading whitespace
        while(isspace(*input_line))
            ++input_line;
        // trim trailing whitespace
        len = strlen(input_line);
        for(x = len -1; x >= 0; x--)
        {
            if(isspace(input_line[x]))
                input_line[x] = '\x0';
            else
                break;
        }

        //skip comments and blank lines
        if(input_line[0] == '#' || input_line[0] == '\x0' || input_line[0] == '\n')
            continue;
        //get the variable name
        varname = strtok(input_line,"=");
        if(varname == NULL)
        {
            fprintf(stdout, "No variable name specified in config file %s - Line %d\n", filename, line);
            return ERROR;
        }
        //get the variable value
        varvalue = strtok(NULL,"\n");
        if(varvalue == NULL)
        {
            fprintf(stdout, "No variable name specified in config file %s - Line %d\n", filename, line);
            return ERROR;
        }
        else if(!strcmp(varname,"server_address"))
        {
            strncpy(server_address, varvalue, sizeof(server_address)-1);
            server_address[sizeof(server_address)-1] = '\0';
#ifdef _XNRPE_DEBUG
            fprintf(stdout,"GET %s=%s\n",varname,varvalue);
#endif
        }
        else if(!strcmp(varname,"server_port"))
        {
            server_port = atoi(varvalue);
            if(server_port < 1024)
            {
                fprintf(stdout,"Invalid port number specified in config file '%s' - Line %d\n",filename, line);
                return ERROR;
            }
#ifdef _XNRPE_DEBUG
            fprintf(stdout,"GET %s=%s\n",varname,varvalue);
#endif // _XNRPE_DEBUG
        }
        else if(!strcmp(varname,"localhost"))
        {
            bzero(local_host, NI_MAXHOST);
            strncpy(local_host, varvalue, sizeof(local_host)-1);
            local_host[sizeof(local_host)-1] = '\0';
#ifdef _XNRPE_DEBUG
            fprintf(stdout,"GET %s=%s\n",varname,varvalue);
#endif // _XNRPE_DEBUG
        }
        else if(!strcmp(varname, "sock_send_recv_timeout"))
        {
            sock_send_recv_timeout = atoi(varvalue);
            if(sock_send_recv_timeout < 0)
            {
                sock_send_recv_timeout = DEFAULT_SOCK_SEND_RECV_TIMEOUT;
            }
#ifdef _XNRPE_DEBUG
            fprintf(stdout, "GET %s=%s\n",varname,varvalue);
#endif // _XNRPE_DEBUG
        }
        else if(!strcmp(varname, "heartbeat_time"))
        {
            heartbeat_time = atoi(varvalue);
            if(heartbeat_time < 0)
            {
                heartbeat_time = DEFAULT_HEARTBEAT_TIME;
            }
#ifdef _XNRPE_DEBUG
            fprintf(stdout, "GET %s=%s\n",varname,varvalue);
#endif // _XNRPE_DEBUG
        }
        else if(!strcmp(varname, "report_time"))
        {
            report_time = atoi(varvalue);
            if(report_time < 0)
            {
                report_time = DEFAULT_HEARTBEAT_TIME;
            }
#ifdef _XNRPE_DEBUG
            fprintf(stdout, "GET %s=%s\n",varname,varvalue);
#endif // _XNRPE_DEBUG
        }
        else if(strstr(input_line, "command["))
        {
            temp_buffer = strtok(varname,"[");
            temp_buffer = strtok(NULL,"]");
            if(temp_buffer == NULL)
            {
                fprintf(stdout, "Invalid command specified in config file '%s' - Line %d\n",filename, line);
                return ERROR;
            }
            add_command(temp_buffer, varvalue);
#ifdef _XNRPE_DEBUG
            fprintf(stdout, "GET %s=%s\n",temp_buffer, varvalue);
#endif // _XNRPE_DEBUG
        }
        else
        {
            fprintf(stdout, "Unknown option specified in config file '%s' - Line %d\n",filename, line);
            continue;
        }
    }
    fclose(fp);
    return OK;
}

#ifdef _TEST
int main(int argc,char **argv)
{
    char *command="../plugins/check_disk_io 0001";
    char buf[MAX_INPUT_BUFFER]={0};
    int len = my_system(command, buf);

    printf("%s\n", buf);
    printf("%d\n", len);
    return 0;
}
#else
int main(int argc,char **argv)
{
    struct sigaction action;
    int result = OK;
    int epfd;
    pthread_t tid,tid1;
    char buffer[MAX_INPUT_BUFFER];

    result = process_arguments(argc, argv);
    if(result != OK || show_help == true)
    {
        usage(result);
        exit(ERROR);
    }
#ifdef _XNRPE_DEBUG
    printf("GET:config_file=%s\n", config_file);
#endif // _XNRPE_DEBUG

    if (config_file[0] != '/')
    {
        strncpy(buffer, config_file, sizeof(buffer));
        buffer[sizeof(buffer)-1] = '\x0';
        strcpy(config_file, "");
        getcwd(config_file,sizeof(config_file));
        strncat(config_file, "/", sizeof(config_file) - 2);
        config_file[sizeof(config_file) - 1] = '\x0';
        //append the config file to the path
        strncat(config_file, buffer, sizeof(config_file) - strlen(config_file) - 1);
        config_file[sizeof(config_file)-1]='\x0';
    }

    //read the config file
    result = read_config_file(config_file);
    /*****/

    action.sa_handler = handle_pipe;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);

    /*int epfd = epoll_create(EPOLL_LEN);
    if(epfd < 0)
    {
        perror("epoll_create");
        return ERROR;
    }
    if((fd = start_connect(host, DEFAULT_SERVER_PORT)) < 0)
    {
        perror("socket error");
        return ERROR;
    }
    setnonblocking(fd);
    event.data.fd = fd;
    event.enents = EPOLLIN|EPOLLET;
    if( 0 != epoll_ctrl(epfd, EPOLL_CTL_ADD, fd, &event))
    {
        perror("epoll_ctrl");
        close(fd);
        return ERROR;
    }
    */
    ARGS args;
    ARGS args1;
    args.time=heartbeat_time;
    //strcpy(args.cmd,"check_disk");
    //strcpy(args.cmd,"check_cpu");
    strcpy(args.cmd,"ACK");
    args1.time=heartbeat_time;
    strcpy(args1.cmd,"ACK");

    int ret = pthread_create(&tid, NULL, task, (void *)&args); //timer  heartbeat
    if(ret)
    {
        perror("pthread_create  threard_heart_beat Fail!");
        return -1;
    }

    int ret1 = pthread_create(&tid1, NULL, report_task, (void *)&args1);
    if(ret1)
    {
        perror("pthread_create  threard_heart_beat Fail!");
        return -1;
    }
    //register signal
    signal(SIGINT,fun_int);
    signal(SIGTERM,fun_term);

    while(signal_flag_int&&signal_flag_term)
    {
        sleep(1);
    }
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    pthread_cancel(tid1);
    pthread_join(tid1, NULL);
    free_memory();
    fprintf(stdout, "\nexit success!\n");
    return 0;
}
#endif
