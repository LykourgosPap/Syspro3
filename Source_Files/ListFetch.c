#include "ListFetch.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void thread_list(void *args1) {
    args arguments = *(args*) args1;
    char buf[512];
    usleep(100);
    if (read(arguments.sock, buf, 512) < 0)
        perror_exit("read");
    char buf2[512];
    sprintf(buf2, "ls -Rp %s", arguments.dof);
    FILE *ls = popen(buf2, "r");
    if (!strncmp(buf, "LIST", 4)) {
        while (fgets(buf, sizeof (buf), ls) != 0) {
            if (buf[0] != '\n') {
                if (write(arguments.sock, buf, strlen(buf) + 1) < 0)
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
    close(arguments.sock); /* Close socket */
}

void thread_fetch(void *args1) {
    args arguments = *(args*) args1;
    FILE *myfile = fopen(arguments.dof, "r");
    char buf[512];
    char buf2[512];
    while (fgets(buf, sizeof (buf), myfile) != 0) {
            if (write(arguments.sock, buf, strlen(buf) + 1) < 0)
                perror_exit("write");

    }
    printf("Closing connection.\n");
    close(arguments.sock); /* Close socket */
}
