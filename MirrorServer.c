#include <stdio.h>
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h> /* gethostbyaddr */
#include <stdlib.h> /* exit */
#include <string.h> /* strlen */

void perror_exit(char *message);
void Mirror_Manager(void *new_sock, struct sockaddr *server, char dof[128], int delay);

int main(int argc, char *argv[]) {
    int port, sock, client_sock, i,*new_sock, c, thread_count=0;
    char buf[256], *token;
    struct sockaddr_in server, client;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;
    int nt = atoi(argv[3]);
    pthread_t *mirror_manager;
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

    /*server.sin_family = AF_INET; /* Internet domain */
   /* memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port); /* Server port */
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
        while (read(client_sock, buf, 256) == 0);
        if (!strncmp(buf, "END", 3))
            break;
        letters = atoi(buf);
        write(client_sock, "ok", 2);
        while (read(client_sock, buf, letters) == 0);
        strtok(buf, ":");
        if ((rem = gethostbyname(buf)) == NULL){
                herror("gethostbyname");
                exit(1);
        }
        memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
        token = strtok(NULL, ":");
        port = atoi(token);
        server.sin_port = htons(port); /* Server port */
                /* Initiate connection */

        new_sock = malloc(sizeof (int));
        if ((*new_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
            perror_exit("socket");
        token = strtok(NULL, ":");
        char dof[128];
        strcpy(dof, token);
        token = strtok(NULL, ":");
        int delay = atoi(buf);
        thread_count++;
        mirror_manager = realloc(mirror_manager, thread_count * sizeof(pthread_t));
        if (connect(*new_sock, serverptr, sizeof (server)) < 0)
            perror("connect");
        if (pthread_create(&mirror_manager[thread_count-1], NULL, Mirror_Manager, (void*) new_sock, &server, dof , delay) < 0) {
            perror("could not create thread");
            return -1;
        }
        write(client_sock, "ok", 2);
    }

    pthread_join(*mirror_manager, NULL);
}

void perror_exit(char *message) {

    perror(message);
    exit(EXIT_FAILURE);
}

void Mirror_Manager(void *new_sock, struct sockaddr *server, char dof[128], int delay) {
    int sock = *(int*) new_sock;
    write(sock, "LIST", 4);
    char buf[1];
    while (1) {
        while (read(sock, buf, 1) != 0) {
            printf("%c", buf[0]);
                    //write(sock, "bla", 3);
        }
    }
    close(sock); /* Close socket */
}