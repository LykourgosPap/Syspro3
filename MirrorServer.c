#include <stdio.h>
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h> /* gethostbyaddr */
#include <stdlib.h> /* exit */
#include <string.h> /* strlen */

void perror_exit(char *message);
void Mirror_Manager(void *new_sock, char *ip_ad, int port);

int main(int argc, char *argv[]) {
    int port, sock, i,*new_sock;
    //char buf[256];
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;
    int nt = atoi(argv[3]);
    pthread_t workers[nt];
    if (argc != 4) {
        printf("Please give host name and port number\n");
        exit(1);
    }
    port = atoi(argv[2]);
        /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    /* Find server address */
    if ((rem = gethostbyname(argv[1])) == NULL) {
        herror("gethostbyname");
        exit(1);
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port); /* Server port */
    /* Initiate connection */
    if (connect(sock, serverptr, sizeof (server)) < 0)
        perror_exit("connect");
    new_sock = malloc(sizeof (int));
    *new_sock = sock;

    printf("Connecting to %s port %d\n", argv[1], port);
    if (pthread_create(&workers[0], NULL, Mirror_Manager, (void*) new_sock, (char*) argv[1], nt) < 0) {
        perror("could not create thread");
        return -1;
    }
    pthread_join(workers[0], NULL);
}

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void Mirror_Manager(void *new_sock, char *ip_ad, int port) {
    int sock = *(int*) new_sock;

    write(sock, "LIST", 4);
    char buf[1];
    while (1) {
        while (read(sock, buf, 1) != 0){
            printf("%c", buf[0]);
            //write(sock, "bla", 3);
        }
    }
    close(sock); /* Close socket */
}