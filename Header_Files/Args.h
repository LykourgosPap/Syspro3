#ifndef ARGS_H
#define ARGS_H

#include <netinet/in.h> /* internet sockets */

typedef struct arguments{
    int sock;
    char ip4[128];
    char dof[256];
    int delay;
    char port[9];
    int ID;
}args;

#endif /* ARGS_H */

