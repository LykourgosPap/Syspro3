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

void perror_exit(char *message);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    char dir[256];
    strcpy(dir, argv[1]);
     
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
    server.sin_port = htons( 8888 );
     
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
     
     
    //Accept and incoming connection

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  thread_list , (void*) new_sock, dir) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        pthread_join( sniffer_thread , NULL);
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}

/*
void thread_list(void *newsock) {
    int sock = *(int*) newsock;
    char buf[512];
    usleep(100);
    if (read(sock, buf, 512) < 0)
        perror_exit("read");
    FILE *ls = popen("ls -Rp", "r");
    char buf2[512];
    if (!strncmp(buf, "LIST", 4)) {
        while (fgets(buf, sizeof (buf), ls) != 0) {
            if (buf[0] != '\n') {
                if (write(sock, buf, strlen(buf) + 1) < 0)
                    perror_exit("write");
            }
            memset(buf, 0, 512);
        }
    }
    if (!strncmp(buf, "FETCH", 5)){
        ;
    }
    printf("Closing connection.\n");
    pclose(ls);
    close(sock); 
}

void thread_fetch(void *newsock, char file[512]) {
    int sock = *(int*) newsock;
    FILE *myfile = fopen(file, "r");
    char buf[512];
    char buf2[512];
    while (fgets(buf, sizeof (buf), myfile) != 0) {
            if (write(sock, buf, strlen(buf) + 1) < 0)
                perror_exit("write");

    }
    printf("Closing connection.\n");
    close(sock); 
}*/

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}