#ifndef STOIVA_H
#define STOIVA_H

typedef struct stoiv{
    char dirorfile[1000][256];
    int items;
}stoiva;

void stoivinit(stoiva *st);
void stoivadd(stoiva *st, char diro[256]);
void stoivrem(stoiva *st, char (*)[256]);
int stoivfull(stoiva st);
int stoivempty(stoiva st);

#endif /* STOIVA_H */

