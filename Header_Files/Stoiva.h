#ifndef OURA_H
#define OURA_H

typedef struct our{
    char dirorfile[1000][256];
    int items;
}oura;

void ourinit(stoiva *st);
void ouradd(stoiva *st, char diro[256]);
void ourrem(stoiva *st, char (*)[256]);
int ourfull(stoiva st);
int ourempty(stoiva st);

#endif /* STOIVA_H */

