#include "mylist.h"

void mylinit(mylist **list){
    *list = malloc(sizeof(mylist));
    list[0]->next = 0;
}

void myladd(mylist *list){
    mylist *temp =list;
    while (temp->next != 0)
        temp = temp->next;
    temp->next = malloc(sizeof(mylist));
    temp->next->next = 0;
}

void myldel(mylist **list)
{
   /* deref head_ref to get the real head */
   mylist* current = *list;
   mylist* next;
 
   while (current != 0) 
   {
       next = current->next;
       free(current);
       current = next;
   }
   
   /* deref head_ref to affect the real head back
      in the caller. */
   free(*list);
}

pthread_t *getthread(mylist *list, int n){
    mylist *temp = list;
    int i;
    for (i=0; i<n; i++){
        if (temp->next != 0)
            temp=temp->next;
    }
    return &temp->lthread;
}