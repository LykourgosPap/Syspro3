#include <stdio.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* gethostbyaddr */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <signal.h> /* signal */
#include <string.h> //strlen
#include "MirrorManager.h" /*list,fetch*/
#include "Global.h"
#include "Args.h"
#include "ListFetch.h"

void perror_exit(char *message);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    int port=atoi(argv[2]);
    struct sockaddr_in server , client;
    args *arguments;
    arguments = malloc(sizeof(args));
    strcpy(arguments->dof, argv[4]);
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept incoming connection
 
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Client accepted.\n"); 
        pthread_t sniffer_thread;
        arguments->sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  thread_list , (void*) arguments) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        pthread_join( sniffer_thread , NULL);
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}


void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}