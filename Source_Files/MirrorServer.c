#include <stdio.h>
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h> /* gethostbyaddr */
#include <stdlib.h> /* exit */
#include <string.h> /* strlen */
#include "MirrorManager.h" /*MirrorManager*/
#include "Oura.h"
#include "Args.h"
#include "Global.h"
#include "mylist.h"

oura myoura;
pthread_mutex_t mymutex;
pthread_cond_t mycond;
int tflag;

void perror_exit(char *message);

int main(int argc, char *argv[]) {
    int port, sock, client_sock, i, c, thread_count=0;
    char buf[256], *token, dof[128];
    struct sockaddr_in server, client;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;
    int nt = atoi(argv[6]);
    struct arguments *args;
    mylist *mirror_manager;
    pthread_t workers[nt];
    ourinit(&myoura);
    pthread_mutex_init(&mymutex, NULL);
    pthread_cond_init (&mycond, NULL);
    if (argc != 7) {
        printf("Please give host name and port number\n");
        exit(1);
    }
    
    tflag = 0;
    port = atoi(argv[2]);
        /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
    
    
        //Bind
    if( bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(sock , 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
      
    //Accept incoming connection
    memset(buf, 0, 256);
    client_sock = accept(sock, (struct sockaddr *) &client, (socklen_t*) & c);
    while (1) {
        int letters = 0;
        while (read(client_sock, buf, 256) <= 0);
        if (!strncmp(buf, "END", 3))
            break;
        letters = atoi(buf);
        write(client_sock, "ok", 2);
        while (read(client_sock, buf, letters) <= 0);
        token = strtok(buf, ":");
        if ((rem = gethostbyname(buf)) == NULL){
                herror("gethostbyname");
                write(client_sock, "ok", 2);
                continue;
        }
        args = malloc(sizeof(struct arguments));
        strcpy(args->ip4, token);
        memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
        token = strtok(NULL, ":");
        port = atoi(token);
        strcpy(args->port, token);
        server.sin_port = htons(port); /* Server port */
                /* Initiate connection */

        if ((args->sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
            perror_exit("socket");
        token = strtok(NULL, ":");
        strcpy(args->dof, token);
        token = strtok(NULL, ":");
        args->delay = atoi(token);
        thread_count++;
        if (thread_count == 1)
                mylinit(&mirror_manager);
        else
            myladd(mirror_manager);
        args->ID = thread_count;
        if (connect(args->sock, serverptr, sizeof (server)) < 0)
            perror("connect");
        pthread_t *temp= getthread(mirror_manager, thread_count);
        if (pthread_create(temp, NULL, Mirror_Manager, (void*) args) < 0) {
            perror("could not create thread");
            return -1;
        }
        if (write(client_sock, "ok", 2) < 0)
            perror_exit("last write");
    }
    
    i=0;
    do {
        args = malloc(sizeof(struct arguments));
        strcpy(args->dof, argv[4]);
        if (pthread_create(&(workers[i]), NULL, thread_fetch, (void*) args) < 0) {
            perror("could not create thread");
        }
        i++;
    }while(i < nt);
    
    while (tflag != -1){
        if (tflag == thread_count)
            tflag = -1;
    }
    
    i=0;
    do {
        pthread_join(workers[i], NULL);
        i++;
    }while(i < nt);
    write (client_sock, "END\0", 5);
    pthread_mutex_destroy(&mymutex);
    pthread_cond_destroy(&mycond);
    myldel(&mirror_manager);
}

void perror_exit(char *message) {

    perror(message);
    exit(EXIT_FAILURE);
}

