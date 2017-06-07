#include "ListFetch.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void thread_list(void *args1) {
    args *arguments = (args*) args1;
    char buf[512];
    usleep(100);
    char *token;
    int flag=0;
    
    while (read(arguments->sock, buf, 512) <= 0);
    
    /* if is a fetch request send file */
    
    if (!strncmp(buf, "FETCH", 5)) {
        arguments->delay = buf[5] - '0';
        
        if (!flag){
            sleep(arguments->delay);
            flag=1;
        }
        FILE *filetosend = fopen(&buf[6], "r");     
        if (filetosend == NULL) {
            printf("cannot open file");
        }
        while (fgets(buf, sizeof (buf), filetosend) != 0) {
            if (write(arguments->sock, buf, strlen(buf)) < 0)
                perror_exit("write");
            memset(buf, 0, 512);
        }
        fclose(filetosend);
    }
    
    /* if list ls the directory that content server has for mirroring */
    if (!strncmp(buf, "LIST", 4)) {
        char buf2[512];
        sprintf(buf2, "ls -Rp %s", arguments->dof);
        FILE *ls = popen(buf2, "r");

        while (fgets(buf, sizeof (buf), ls) != 0) {
            if (buf[0] != '\n') {
                if (write(arguments->sock, buf, strlen(buf) + 1) < 0)
                    perror_exit("write");
            }
            memset(buf, 0, 512);
        }
        write(arguments->sock, "END\0", 4);
        pclose(ls);
    }
    printf("closing connection...\n");
    close(arguments->sock); /* Close socket */
    //free(arguments);
}


