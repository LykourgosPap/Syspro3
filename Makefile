IDIR = ./Header_Files
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./EXE_Files
SDIR=./Source_Files

LIBS=-pthread

_DEPS = Args.h MirrorManager.h Stoiva.h Global.h ListFetch.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = MirrorInitiator.o MirrorServer.o ContentServer.o MirrorManager.o Stoiva.o ListFetch.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_SRC = MirrorInitiator.c MirrorServer.c ContentServer.c MirrorManager.c Stoiva.c ListFetch.c
SRC = $(patsubst %,$(SDIR)/%,$(_SRC))

all : MirrorInitiator MirrorServer ContentServer

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

MirrorInitiator: $(ODIR)/MirrorInitiator.o 
	gcc -o $@ $^ $(LIBS)

MirrorServer: $(ODIR)/MirrorServer.o $(ODIR)/MirrorManager.o $(ODIR)/Stoiva.o
	gcc -o $@ $^ $(LIBS)

ContentServer: $(ODIR)/ContentServer.o $(ODIR)/ListFetch.o
	gcc -o $@ $^ $(LIBS)

RunMirrorServer: MirrorServer
	./MirrorServer -p 1234 -m ./Dls/ -w 10

RunMirrorInitiator: MirrorInitiator
	./MirrorInitiator -n 192.168.8.14 -p 1234 -s 192.168.8.155:8888:./:5,192.168.8.14:8888:./:5

RunContentServer: ContentServer
	./ContentServer -p 8888 -d ./

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o MirrorInitiator MirrorServer ContentServer 
