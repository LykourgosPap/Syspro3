#ifndef MYLIST_H
#define MYLIST_H

#include <stdlib.h>
#include <pthread.h>

typedef struct myl{
    pthread_t lthread;
    struct myl *next;
}mylist;

void mylinit(mylist **list);
void myladd(mylist *list);
void myldel(mylist **list);
pthread_t *getthread(mylist *list, int n);

#endif /* MYLIST_H */

