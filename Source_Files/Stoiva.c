#include "Stoiva.h"
#include <string.h>
#include "stdio.h"

void stoivinit(stoiva *st){
    st->items=-1;
}

int stoivfull(stoiva st){
    if (st.items == 999)
        return 1;
    else
        return 0;
}

int stoivempty(stoiva st){
    if (st.items == -1)
        return 1;
    else
        return 0;
}



void stoivadd(stoiva *st, char diro[256]){
    if (!stoivfull(*st)){
        st->items++;
        strcpy(st->dirorfile[st->items], diro);
    }
    else
        printf("Cannot add stack is full\n");
}

void stoivrem(stoiva *st, char (*diro)[256]){
    if (!stoivempty(*st)){
        strcpy(*diro, st->dirorfile[st->items]);
        st->items--;
    }
    else{
        printf("Cannot remove stack is empty\n");
    }
}
