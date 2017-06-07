#ifndef OURA_H
#define OURA_H

typedef struct our{
    char dirorfile[1000][256];
    int items;
    int front;
    int end;
}oura;

void ourinit(oura *st);
void ouradd(oura *st, char diro[256]);
char *ourrem(oura *st);
int ourfull(oura st);
int ourempty(oura st);

#endif /* OURA_H */

