#include <stdio.h>
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h> /* gethostbyaddr */
#include <stdlib.h> /* exit */
#include <string.h> /* strlen */
#include "MirrorManager.h" /*MirrorManager*/
#include "Stoiva.h"
#include "Args.h"
#include "Global.h"

stoiva mystoiva;
pthread_mutex_t mymutex;
pthread_cond_t mycond;

void perror_exit(char *message);

int main(int argc, char *argv[]) {
    int port, sock, client_sock, i, c, thread_count=0;
    char buf[256], *token, dof[128];
    struct sockaddr_in server, client;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;
    int nt = atoi(argv[6]);
    struct arguments *args;
    pthread_t *mirror_manager = malloc(sizeof(pthread_t)), *temp;
    pthread_t workers[nt];
    stoivinit(&mystoiva);
    pthread_mutex_init(&mymutex, NULL);
    pthread_cond_init (&mycond, NULL);
    if (argc != 7) {
        printf("Please give host name and port number\n");
        exit(1);
    }
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
        strtok(buf, ":");
        if ((rem = gethostbyname(buf)) == NULL){
                herror("gethostbyname");
                write(client_sock, "ok", 2);
                continue;
        }
        args = malloc(1*sizeof(struct arguments));
        memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
        token = strtok(NULL, ":");
        port = atoi(token);
        server.sin_port = htons(port); /* Server port */
                /* Initiate connection */

        if ((args->sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
            perror_exit("socket");
        token = strtok(NULL, ":");
        strcpy(args->dof, token);
        token = strtok(NULL, ":");
        args->delay = atoi(token);
        thread_count++;
        temp = realloc(mirror_manager, thread_count * sizeof(pthread_t));
        mirror_manager = temp;
        args->server = serverptr;
        if (connect(args->sock, args->server, sizeof (server)) < 0)
            perror("connect");
        if (pthread_create(&(mirror_manager[thread_count-1]), NULL, Mirror_Manager, (void*) args) < 0) {
            perror("could not create thread");
            return -1;
        }
        if (write(client_sock, "ok", 2) < 0)
            perror_exit("last write");
    }
    for (i=0; i<nt; i++) {
        strcpy(args->dof, argv[4]);
        if (pthread_create(&(workers[i]), NULL, thread_fetch, (void*) args) < 0) {
            perror("could not create thread");
        }
    }
    pthread_join(*mirror_manager, NULL);
    pthread_mutex_destroy(&mymutex);
    pthread_cond_destroy(&mycond);
}

void perror_exit(char *message) {

    perror(message);
    exit(EXIT_FAILURE);
}

