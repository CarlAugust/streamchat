#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>

#include <config.h>

int main(void)
{
    int client_fd;
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

    for(;;)
    {
        char buffer[4] = "abs";
        send(client_fd, buffer, 4, 0);
        int n = read(client_fd, buffer, 4);
        if (n > 0)
        {
            printf("%s\n", buffer);
        }
        else
        {
            perror("read");
        }
    }

    return EXIT_SUCCESS;
}