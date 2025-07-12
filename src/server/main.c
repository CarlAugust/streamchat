#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <signal.h>
#include <errno.h>
#include <config.h>
#include <string.h>



int socket_setNonBlocking(int fd);
void async_serverLoop(int server_fd, struct sockaddr* addr, socklen_t addr_len, struct epoll_event* events, int epfd);

int server_fd;

volatile sig_atomic_t shutdown_requested = 0;
static void handle_sigint(int sig)
{
    shutdown_requested = 1;
    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);
}

int main(void)
{  
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if(socket_setNonBlocking(server_fd) == -1)
    {
        close(server_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = INADDR_ANY;
    addr_in.sin_port = htons(SERVER_PORT);

    struct sockaddr* addr = (struct sockaddr *)&addr_in;
    socklen_t addr_len = sizeof(addr_in);

    if(bind(server_fd, addr, addr_len) == -1)
    {
        perror("Bind");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if(listen(server_fd, MAX_CLIENTS))
    {
        perror("Listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    int epfd = epoll_create(1);
    struct epoll_event ev = {.data = {.fd = server_fd},
                             .events = EPOLLIN | EPOLLOUT | EPOLLET };
    
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
    {
        perror("epoll_ctl");
        close(server_fd);
        close(epfd);
        return EXIT_FAILURE;
    }
    
    struct epoll_event events[MAX_EVENTS];
    while (!shutdown_requested) 
    {
        async_serverLoop(server_fd, addr, addr_len, events, epfd);
    }

    close(epfd);

    return EXIT_SUCCESS;
}

int socket_setNonBlocking(int fd)
{
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    return 0;
}

// return 0 or -1 for disconnets and 1 for success
int handle_clientMessage(int client_fd)
{
    char message[MAX_MESSAGE_SIZE];
    int read_c;
    if ((read_c = read(client_fd, message, MAX_MESSAGE_SIZE)) < 1)
    {
        return read_c;
    }
    const char* command_msg = "/msg";
    const char* command_chgusrn = "/chgusrn";

    char format[20];
    snprintf(format, sizeof(format), "%%%zus %%%zus", MAX_COMMAND_SIZE, MAX_MESSAGE_SIZE); // Example "%20s %400s"

    char command[MAX_COMMAND_SIZE];
    char message_content[MAX_MESSAGE_CONTENT];
    int scan_c;
    scan_c = sscanf(message, format, command, message_content);
    if (scan_c != 2)
    {
        if(send(client_fd, "Err", 4, 0) == -1)
        {
            return -1;
        }
        return 1;
    }

    if(strncmp(command, command_msg, strlen(command_msg)))
    {
        clients_broadcast(client_fd, message_content);
    }
    else if (strncmp(command, command_msg, strlen(command_chgusrn)))
    {
        client_changeUsername(client_fd, message_content);
    }
}

void async_serverLoop(int server_fd, struct sockaddr* addr, socklen_t addr_len, struct epoll_event* events, int epfd)
{
    int nfds;
    if ((nfds = epoll_wait(epfd, events, MAX_EVENTS, 1000)) == -1)
    {
        perror("epoll_wait");
        close(server_fd);
        close(epfd);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < nfds; i++) 
    {
        if (events[i].data.fd == server_fd)
        {
            int client_fd = accept(server_fd, addr, &addr_len);
            if (client_fd == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                perror("accept");
                continue;
            }

            if (socket_setNonBlocking(client_fd) == -1)
            {
                close(client_fd);
                continue;
            }

            struct epoll_event client_ev;
            client_ev.data.fd = client_fd;
            client_ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLET;
            if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1)
            {
                perror("epoll_ctl add client");
                close(client_fd);
            }
        }
        else if (events[i].events & EPOLLIN)
        {
            int client_fd = events[i].data.fd;
            if (handle_clientMessage(client_fd) < 1)
            {
                epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
                close(client_fd);
            }
        }
        else
        {
            printf("Unexpected event from client %d\n", events[i].data.fd);
        }
    }
}