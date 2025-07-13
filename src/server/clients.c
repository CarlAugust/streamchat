#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <clients.h>
#include <stdio.h>

void client_changeUsername(Client* client, const char* username)
{
    strncpy(client->username, username, MAX_USERNAME_SIZE);
}

void client_close(Client* client)
{
    close(client->client_fd);
    client->connected = false;
}

Clients* clients_create()
{
    Clients* clients = calloc(1, sizeof(Clients));
    if (clients == NULL)
    {
        return NULL;
    }
    return clients;
}

int clients_add(Clients* clients, int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i]->connected == false)
        {
            clients[i]->client_fd = client_fd;
            clients[i]->connected = true;
            strncpy(clients[i]->username, "Unknown", MAX_USERNAME_SIZE);
            return 0;
        }
    }
    return -1;
}

Client* clients_findClientByFd(Clients* clients, int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i]->connected == true && clients[i]->client_fd == client_fd)
        {
            return clients[i];
        }
    }
    return NULL;
}

int clients_broadcast(Clients* clients, const char* message)
{
    printf("Broadcasting\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i]->connected == true)
        {
            if(send(clients[i]->client_fd, message, strlen(message), 0) == -1)
            {
                perror("send");
            }
        }
    }
    return 0;
}