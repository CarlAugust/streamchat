#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include <config.h>

int client_fd;

volatile sig_atomic_t shutdown_requested = 0;
static void handle_sigint(int sig)
{ 
    printf("Closed\n");
    shutdown_requested = 1;
    close(client_fd);
}

int loop_recieve(int client_fd)
{
    while(!shutdown_requested)
    {
        char buffer[MAX_MESSAGE_SIZE] = {0};
        int n = read(client_fd, buffer, MAX_MESSAGE_SIZE);
        if (n > 0)
        {
            printf("SERVER: %s\n", buffer);
        }
        else
        {
            perror("read");
            shutdown_requested = 1;
            return -1;
        }
    }
    return 0;
}

int loop_readStdin(int client_fd)
{
    while(!shutdown_requested)
    {
        char input[MAX_MESSAGE_SIZE] = {0};
        fgets(input, MAX_MESSAGE_SIZE, stdin);

        if(send(client_fd, input, MAX_MESSAGE_SIZE, 0) == -1)
        {
            perror("Send");
            return -1;
        }
    }
    return 0;
}

void* thread_readStdinLoop(void* arg)
{
    int* client_fd = (int* )(arg);
    loop_readStdin(*client_fd);
    return NULL;
}

int main(void)
{

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }


    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    addr_in.sin_port = htons(SERVER_PORT);

    struct sockaddr* addr = (struct sockaddr *)&addr_in;
    socklen_t addr_len = sizeof(addr_in);

    if (connect(client_fd, addr, addr_len) == -1)
    {
        perror("connect");
        close(client_fd);
        return EXIT_FAILURE;
    }

    pthread_t stdin_thread;
    pthread_create(&stdin_thread, NULL, thread_readStdinLoop, &client_fd);   
    loop_recieve(client_fd);
    pthread_join(stdin_thread, NULL);
    close(client_fd);
    return EXIT_SUCCESS;
}