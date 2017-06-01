#ifndef STOIVA_H
#define STOIVA_H

typedef struct stoiv{
    char dirorfile[10000][256];
    int items;
}stoiva;

void stoivinit(stoiva *st);
void stoivadd(stoiva *st, char diro[256]);
char *stoivrem(stoiva *st);
int stoivfull(stoiva st);
int stoivempty(stoiva st);

#endif /* STOIVA_H */

