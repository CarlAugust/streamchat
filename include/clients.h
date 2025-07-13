#pragma once

#include <config.h>

/*
    - Client_fd, should be both in events and a client struct,
    and the fd will be closed in this struct, but removed from 
    events seperately

    - Connected, for easy dynamic control over clients 
*/
typedef struct Client {
    int client_fd;
    int connected;
    char username[MAX_USERNAME_SIZE];
} Client;

void client_changeUsername(Client* client, const char* username);
void client_close(Client* client);

typedef Client Clients[MAX_CLIENTS];

Clients* clients_create();
int clients_add(Clients* clients, int client_fd);

int clients_broadcast(Clients* clients, const char* message);
Client* clients_findClientByFd(Clients* clients, int client_fd);
