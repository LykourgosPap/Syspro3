#ifndef ARGS_H
#define ARGS_H

#include <netinet/in.h> /* internet sockets */

typedef struct arguments{
    int sock;
    struct sockaddr *server;
    char dof[256];
    int delay;
}args;

#endif /* ARGS_H */

