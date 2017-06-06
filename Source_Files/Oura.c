#include "Oura.h"
#include <string.h>
#include "stdio.h"

void ourinit(oura *st){
    st->items=0;
    st->front=0;
    st->end=0;
}

int ourfull(oura st){
    if (st.items == 1000)
        return 1;
    else
        return 0;
}

int ourempty(oura st){
    if (st.items == 0)
        return 1;
    else
        return 0;
}



void ouradd(oura *st, char diro[256]){
    if (!ourfull(*st)){
        st->items++;
        strcpy(st->dirorfile[st->end], diro);
        st->end++;
    }
    else
        printf("Cannot add queue is full\n");
}

char *ourrem(oura *st){
    if (!ourempty(*st)){
        char *stringg = malloc(strlen(st->dirorfile[st->front])+1);
        strcpy(stringg, st->dirorfile[st->front]);
        st->front++;
        st->items--;
        return stringg;
    }
    else{
        printf("Cannot remove queue is empty\n");
    }
}
