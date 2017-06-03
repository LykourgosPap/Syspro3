#include <stdio.h>
#include <stdlib.h>
#include "MirrorManager.h"
#include "Stoiva.h"
#include <string.h>
#include "Args.h"
#include "Global.h"

void Mirror_Manager(void *args1) {
    struct arguments *args = (struct arguments*) args1;
    char buf[1];
    int flag=0;
    int i=0;
    char forfetch[256];
    strcpy(forfetch, "bla");
    char curdir[256];
    strcpy(forfetch, "bla");
    if (write(args->sock, "LIST", 4) < 0){
        close(args->sock);
        free(args);
        pthread_exit(NULL);
    }
    while (1) {
        while (read(args->sock, buf, 1) > 0) {
            if (buf[0] == '\n' || buf[0] == '\0'){
                forfetch[i]='\0';
                if (forfetch[i-1] == ':'){
                    forfetch[i-1] = '\0';
                    //printf("%s\n", args->dof);
                    if (!strcmp(args->dof, forfetch)){
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
                    if (strncmp(tempdir, forfetch, strlen(tempdir)-1))
                        strcat(tempdir, forfetch);
                    printf("%s\n", tempdir);
                    pthread_mutex_lock(&mymutex);
                    stoivadd(&mystoiva, tempdir);
                    pthread_mutex_unlock(&mymutex);
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
    char savedir[256];
    strcpy(savedir, arguments->dof);
    while (!stoivempty(mystoiva)) {
        pthread_mutex_lock(&mymutex);
        stoivrem(&mystoiva, &(arguments->dof));
        pthread_mutex_unlock(&mymutex);
        printf("%s\n", arguments->dof);
        if (arguments->dof[strlen(arguments->dof) - 1] == ':') {
            arguments->dof[strlen(arguments->dof)- 1] = '\0';
            strcat(savedir, &(arguments->dof[2]));
            mkdir(savedir, 0777);
            
        }
        
        else if (arguments->dof[strlen(arguments->dof) - 1] == '/'){
            char reallocation[256];
            strcpy(reallocation, savedir);
            //printf("real: %s\n", reallocation);
            //mkdir(reallocation, 0777);
        }
    }
    /*
    FILE *myfile = fopen(arguments.dof, "w");
    char buf[1];
    char buf2[512];
    while (read(arguments.sock, buf, 1) > 0) {
        write(myfile, buf, 1);
    }
    fclose(myfile);*/
    printf("Closing connection.\n");
    close(arguments->sock); /* Close socket */
    //free(arguments);
}


