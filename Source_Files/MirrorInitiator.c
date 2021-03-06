#include <stdio.h>
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h> /* gethostbyaddr */
#include <stdlib.h> /* exit */
#include <string.h> /* strlen */

void perror_exit(char *message);

void main(int argc, char *argv[]) {
    int port, sock, i;
    char buf[256];
    char *ContentServer;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;

    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    /* Find server address */
    if ((rem = gethostbyname(argv[2])) == NULL) {
        herror("gethostbyname");
        exit(1);
    }
    ContentServer = strtok(argv[6], ",");
    port = atoi(argv[4]); /*Convert port number to integer*/
    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port); /* Server port */
    /* Initiate connection */
    if (connect(sock, serverptr, sizeof (server)) < 0)
        perror_exit("connect");
    printf("Connecting to %s port %d\n", argv[2], port);
    
    /* Start writing to MirrorServer */
    
    do {
        char letters[10];       /*Variable used to infrom MirrorServer how many characters will be sent*/
        sprintf(letters, "%d", strlen(ContentServer));
        write(sock, letters, strlen(letters));
        while (read(sock, buf, 2) == 0);
        write(sock, ContentServer, strlen(ContentServer));  /*Write the actual message*/
        ContentServer = strtok(NULL, ",");
        while (read(sock, buf, 2) == 0);
    } while (ContentServer != NULL); 
    
    write(sock, "END", 3);  /* Write END to MirrorServer to signal the end of input */
    
    /* Read information from MirrorServer */
    
    while (1){
        char buf2[1];
        int i=0;
        while (read(sock, buf2, 1) > 0){
            buf[i] = buf2[0];
            i++;
            if (!strncmp(buf, "END\0", 5)){
                printf("All valid files have been copied\n");
                break;
            }
        }
        if (!strncmp(buf, "END\0", 5)){
                printf("All valid files have been copied\n");
                break;
        }
    }
    close(sock);
}

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}