#include <stdio.h>
#include <stdlib.h>
#include "MirrorManager.h"
#include "Stoiva.h"
#include <string.h>
#include "Args.h"
#include "Global.h"

void Mirror_Manager(void *args1) {
    struct arguments args = *(struct arguments*) args1;    
    write(args.sock, "LIST", 4);
    char buf[1];
    int flag=0;
    int i=0;
    printf("%d\n", args.delay);
    char forfetch[256];
    while (1) {
        while (read(args.sock, buf, 1) > 0) {
            if (buf[0] == '\n' || buf[0] == '\0'){
                forfetch[i]='\0';
                if (forfetch[i-1] == ':'){
                    if (!strncmp(args.dof, forfetch, strlen(args.dof))){
                        flag=1;
                    }
                    else
                        flag=0;
                }
                
                if (flag && forfetch[0] != '\n' && forfetch[0] != '\0'){
                    pthread_mutex_lock(&mymutex);
                    stoivadd(&mystoiva, forfetch);
                    printf("Stoiva:%s\n", mystoiva.dirorfile[mystoiva.items-1]);
                    pthread_mutex_unlock(&mymutex);
                }
                i=0;
            }
            
            else{
                forfetch[i]=buf[0];
                i++;
            }
        }
    }
    free(args1);
    close(args.sock); /* Close socket */
}

void thread_list(void *newsock, char dir[256]) {
    int sock = *(int*) newsock;
    char buf[512];
    usleep(100);
    if (read(sock, buf, 512) < 0)
        perror_exit("read");
    char buf2[512];
    sprintf(buf2, "ls -Rp %s", dir);
    FILE *ls = popen(buf2, "r");
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
    close(sock); /* Close socket */
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
    close(sock); /* Close socket */
}

