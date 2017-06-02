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

void thread_fetch(void *args1) {
    args arguments = *(args*) args1;
    char savedir[256];
    strcpy(savedir, arguments.dof);
    sleep(4);
    while (!stoivempty(mystoiva)) {
        pthread_mutex_lock(&mymutex);
        stoivrem(&mystoiva, &(arguments.dof));
        printf("%s\n", arguments.dof);
        pthread_mutex_unlock(&mymutex);
        if (arguments.dof[strlen(arguments.dof) - 1] == ':') {
            arguments.dof[strlen(arguments.dof)- 1] = '\0';
            strcat(savedir, &arguments.dof[2]);
            mkdir(savedir, 0777);
            
        }
        
        else if (arguments.dof[strlen(arguments.dof) - 1] == '/'){
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
    close(arguments.sock); /* Close socket */
}


