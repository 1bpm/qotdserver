/*
 qotdserver
Copyright (C) 2019 Richard Knight

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "qotdserver.h"
#include "logger.h"

const char *port = "17";

struct workerArgs
{
    int socket;
};

void *accept_clients(void *args);
void *service_single_client(void *args);

int serve()
{   
    pthread_t server_thread;
    sigset_t new;
    sigemptyset (&new);
    sigaddset(&new, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &new, NULL) != 0) 
    {
        write_log("Unable to mask SIGPIPE");
        raise(SIGTERM);
    }
    
    if (pthread_create(&server_thread, NULL, accept_clients, NULL) < 0)
    {
        write_log("Could not create server thread");
        raise(SIGTERM);
    }

    pthread_join(server_thread, NULL);
    pthread_exit(NULL);
}


void *accept_clients(void *args)
{
    int serverSocket;
    int clientSocket;
    pthread_t worker_thread;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage *clientAddr;
    socklen_t sinSize = sizeof(struct sockaddr_storage);
    struct workerArgs *wa;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 

    if (getaddrinfo(NULL, port, &hints, &res) != 0)
    {
        write_log("getaddrinfo() failed");
        pthread_exit(NULL);
    }

    for(p = res;p != NULL; p = p->ai_next) 
    {
        if ((serverSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            write_log("Could not open socket");
            continue;
        }

        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            write_log("Socket setsockopt() failed");
            close(serverSocket);
            continue;
        }

        if (bind(serverSocket, p->ai_addr, p->ai_addrlen) == -1)
        {
            write_log("Socket bind() failed");
            close(serverSocket);
            continue;
        }

        if (listen(serverSocket, 5) == -1)
        {
            write_log("Socket listen() failed");
            close(serverSocket);
            continue;
        }

        break;
    }
    
    freeaddrinfo(res);

    if (p == NULL)
    {
        write_log("Could not find a socket to bind to");
        pthread_exit(NULL);
    }

    /* Loop and wait for connections */
    while (1)
    {
        clientAddr = malloc(sinSize);
        if ((clientSocket = accept(serverSocket, (struct sockaddr *) clientAddr, &sinSize)) == -1) 
        {
     
            free(clientAddr);
            write_log("Could not accept() connection");
            continue;
        }
        
        
        
        wa = malloc(sizeof(struct workerArgs));
        wa->socket = clientSocket;

        if (pthread_create(&worker_thread, NULL, service_single_client, wa) != 0) 
        {
            write_log("Could not create a worker thread");
            free(clientAddr);
            free(wa);
            close(clientSocket);
            close(serverSocket);
            pthread_exit(NULL);
        }
    }

    pthread_exit(NULL);
}


void *service_single_client(void *args) {
    struct workerArgs *wa;
    int socket, nbytes;

    wa = (struct workerArgs*) args;
    socket = wa->socket;

    pthread_detach(pthread_self());
    
    // log connection
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(socket, (struct sockaddr *)&addr, &addr_size);
    char log_string[20 + INET_ADDRSTRLEN] = "Connection from ";
    strcat(log_string, inet_ntoa(addr.sin_addr));
    
 
    // send 
    char *tosend = get_quote(&config);
    nbytes = send(socket, tosend, strlen(tosend), 0);
    close(socket);
    free(wa);

    // write log
    write_log(log_string);
    pthread_exit(NULL);

}

