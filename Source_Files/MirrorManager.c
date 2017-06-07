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

/* thread responsible for getting the list of files/folders */

void Mirror_Manager(void *args1) {
    struct arguments *args = (struct arguments*) args1;
    char buf[1];
    int flag=0;
    int i=0;
    char forfetch[256];
    sprintf(forfetch, "LIST%d", args->ID);
    char curdir[256];
    strcpy(curdir, "bla");
    
    /* send LIST and the file/folder to content server */
    if (write(args->sock, forfetch, strlen(forfetch)) < 0){
        close(args->sock);
        free(args);
        pthread_exit(NULL);
    }
    
    if (args->dof[0] == '.'){
        strcpy(curdir, &args->dof[2]);
    }

    else {
        strcpy(curdir, &args->dof[1]);
    }
    while (1) {
        
        /*read all files/folders that were requested*/
        
        
        /* read and construct the string as needed to be saved s*/
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
                        if (i > 3)
                            curdir[i-1] = '/';
                        curdir[i] = '\0';
                    }
                    else
                        flag=0;
                }
                
                /* this is also the construction of the string to be put on buffer */
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
                        pthread_mutex_lock(&mymutex);   //lock mutex
                        ouradd(&myoura, forfetch);      //add file
                        pthread_mutex_unlock(&mymutex); //unlock
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

    pthread_mutex_lock(&mymutex);
    tflag++;                            //counting finished mirror manager threads
    pthread_cond_broadcast(&mycond);   //when a thread that adds to buffer ends signal all worker threads to wake up
    pthread_mutex_unlock(&mymutex);
    close(args->sock); /* Close socket */
    free(args);
}

/* thread responsible for getting the files and creating the folders */

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
    while (1) {
        pthread_mutex_lock(&mymutex);
        while (ourempty(myoura)) {      //if queue is empty
            if (tflag == -1) {          //and all threads responsible for adding to queue are finished close resources and exit
                close(arguments->sock);
                free(arguments);
                pthread_mutex_unlock(&mymutex);
                pthread_exit(NULL);
            }
            pthread_cond_wait(&mycond, &mymutex);
        }
        ouraremove = ourrem(&myoura);
        pthread_mutex_unlock(&mymutex);
        token = strtok_r(ouraremove, ":", &saveptr);    //break string in tokens
        strcpy(arguments->ip4, token);      //first token is ip
        token = strtok_r(NULL, ":", &saveptr);
        strcpy(arguments->port, token);     //second token is port
        token = strtok_r(NULL, ":", &saveptr);
        strcpy(arguments->dof, savedir);    //constructing save directory on arguments->dof
        if (token[1] == '.')
            strcat(arguments->dof, &token[3]);  //if path given is relative we skip . because we allready have from savedir
        else
            strcat(arguments->dof, &token[2]);  //else it is fullpath
        
        /* if current file is folder dont request it just create the folder */
        if (arguments->dof[strlen(arguments->dof)- 1] == '/') {
            char mmkdir[128] = "mkdir -p ";
            strcat(mmkdir, arguments->dof);
            FILE *createdir = popen(mmkdir, "r");   //create recursively all folders needed
            pclose(createdir);
        } 
        
        /* if its regular file request it from content server */
        
        else {
            memset(&hints, 0, sizeof(hints));
            if ((rv = getaddrinfo(arguments->ip4, arguments->port, &hints, &servinfo)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            }
            
            if ((arguments->sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
                perror("socket");
                continue;
            }
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
                strcat(buf2, token);
                if (write(arguments->sock, buf2, strlen(buf2)+1) <0)
                    perror("socket write");
                while (read(arguments->sock, buf, 1) > 0) {
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


