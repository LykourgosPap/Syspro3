#include "Stoiva.h"
#include <string.h>
#include "stdio.h"

void stoivinit(stoiva *st){
    st->items=0;
}

int stoivfull(stoiva st){
    if (st.items == 10000)
        return 1;
    else
        return 0;
}

int stoivempty(stoiva st){
    if (st.items == 0)
        return 1;
    else
        return 0;
}



void stoivadd(stoiva *st, char diro[256]){
    if (!stoivfull(*st)){
        strcpy(st->dirorfile[st->items], diro);
        st->items++;
    }
    else
        printf("Cannot add stack is full\n");
}

char *stoivrem(stoiva *st){
    if (!stoivempty(*st)){
        st->items--;
        return st->dirorfile[st->items+1];
    }
    else{
        printf("Cannot remove stack is empty");
        return '\0';
    }
}
