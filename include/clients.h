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
    char* username;
} Client;

int client_changeName(int client_fd, const char* username);
int client_close();

typedef struct Clients {
    Client* list;
    int count;
    int capacity;   
} Clients;

Clients* clients_create();
int clients_destroy(Clients* clients);
int clients_add(Clients* clients);

int clients_broadcast();
Client* clients_findClientByFd(Clients* client, int client_fd);
