#ifndef ARGS_H
#define ARGS_H

#include <netinet/in.h> /* internet sockets */

struct arguments{
    int sock;
    struct sockaddr *server;
    char dof[128];
    int delay;
};

#endif /* ARGS_H */

