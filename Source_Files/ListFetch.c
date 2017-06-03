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
    while (read(arguments->sock, buf, 512) < 0);
    if (!strncmp(buf, "FETCH", 5)) {
            token = strtok(buf, " ");
            token = strtok(NULL, " ");
            FILE *filetosend = fopen(token, "r");
            while (fgets(buf, sizeof (buf), filetosend) != 0) {
                if (write(arguments->sock, buf, strlen(buf)) < 0)
                    perror_exit("write");
                memset(buf, 0, 512);
            }
            fclose(filetosend);
    }
    
    if (!strncmp(buf, "LIST", 4)) {
        printf("bika\n");
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
        pclose(ls);
    }
    printf("Closing connection.\n");
    write(arguments->sock, "END\0", 4);
    close(arguments->sock); /* Close socket */
    free(arguments);
}


