#ifndef MIRRORMANAGER_H
#define MIRRORMANAGER_H
#include <netinet/in.h> /* internet sockets */

void Mirror_Manager(void *);
void thread_list(void *, char [256]);
void thread_fetch(void *newsock, char file[512]);


#endif /* MIRRORMANAGER_H */

