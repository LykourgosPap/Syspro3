#include <stdio.h>
#include <stdlib.h>
#include "MirrorManager.h"
#include "Oura.h"
#include <string.h>
#include "Args.h"
#include "Global.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void Mirror_Manager(void *args1) {
    struct arguments *args = (struct arguments*) args1;
    char buf[1];
    int flag=0;
    int i=0;
    char forfetch[256];
    sprintf(forfetch, "LIST%d", args->ID);
    char curdir[256];
    strcpy(curdir, "bla");
    if (write(args->sock, forfetch, strlen(forfetch)) < 0){
        close(args->sock);
        free(args);
        pthread_exit(NULL);
    }
    while (1) {
        while (read(args->sock, buf, 1) > 0) {
            if (buf[0] == '\n' || buf[0] == '\0'){
                forfetch[i]='\0';
                if (!strncmp(forfetch, "END\0", 4))
                    break;
                if (forfetch[i-1] == ':'){
                    forfetch[i-1] = '\0';
                    if (!strncmp(args->dof, forfetch, strlen(args->dof))){
                        flag=1;
                        strcpy(curdir, forfetch);
                        curdir[i-1] = '/';
                        curdir[i] = '\0';
                    }
                    else
                        flag=0;
                }
                
                if (flag && forfetch[0] != '\n' && forfetch[0] != '\0'){
                    char tempdir[256];
                    strcpy(tempdir, curdir);
                    if (forfetch[strlen(forfetch)-1] != '/') {
                        if (strncmp(tempdir, forfetch, strlen(tempdir) - 1))
                            strcat(tempdir, forfetch);
                        strcpy(forfetch, args->ip4);
                        strcat(forfetch, ":");
                        sprintf(&forfetch[strlen(forfetch)], "%s", args->port);
                        strcat(forfetch, ":");
                        sprintf(&forfetch[strlen(forfetch)], "%d", args->delay);
                        strcat(forfetch, tempdir);
                        pthread_mutex_lock(&mymutex);
                        ouradd(&myoura, forfetch);
                        pthread_mutex_unlock(&mymutex);
                    }
                }
                i=0;
            }
            
            else{
                forfetch[i]=buf[0];
                i++;
            }
        }
        if (!strncmp(forfetch, "END\0", 4))
            break;
    }
    close(args->sock); /* Close socket */
    free(args);
}

void thread_fetch(void *args1) {
    args *arguments = (args*) args1;
    struct addrinfo hints, *servinfo;
    int rv;
    hints.ai_socktype = SOCK_STREAM;
    char savedir[256];
    char *ouraremove;
    char *token;
    char *saveptr;
    strcpy(savedir, arguments->dof);
    sleep(4);
    while (1) {
        pthread_mutex_lock(&mymutex);
        if (ourempty(myoura)){
            pthread_mutex_unlock(&mymutex);
            break;
        }
        ouraremove = ourrem(&myoura);
        pthread_mutex_unlock(&mymutex);
        token = strtok_r(ouraremove, ":", &saveptr);
        strcpy(arguments->ip4, token);
        token = strtok_r(NULL, ":", &saveptr);
        strcpy(arguments->port, token);
        token = strtok_r(NULL, ":", &saveptr);
        strcpy(arguments->dof, savedir);
        strcat(arguments->dof, &token[1]);
        if (arguments->dof[strlen(arguments->dof)- 1] == '/') {
            mkdir(arguments->dof, 0777);
        } 
        
        else {
            memset(&hints, 0, sizeof(hints));
            if ((rv = getaddrinfo(arguments->ip4, arguments->port, &hints, &servinfo)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            }
            
            if ((arguments->sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
                perror("socket");
                continue;
            }
            //server.sin_port = htons(arguments->port);
            if (connect(arguments->sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
                perror("connect");
                close(arguments->sock);
                continue;
            }
            else {
                FILE *myfile = fopen(arguments->dof, "w");
                if (myfile == NULL) {
                    perror("file error");
                }
                char buf[1];
                char buf2[512];
                strcpy(buf2, "FETCH");
                strcat(buf2, arguments->dof);
                if (write(arguments->sock, buf2, strlen(buf2)+1) <0)
                    perror("socket write");
                while (read(arguments->sock, buf, 1) > 0) {
                                     printf("dsdad %c", buf[0]);
                    fputc(buf[0], myfile);
                }
                fclose(myfile);
            }
            close(arguments->sock);
        }
    }
    printf("Closing connection.\n");
    free(arguments);
}


