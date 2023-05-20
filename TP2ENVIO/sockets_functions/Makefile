ROAD = ../Modulos/
CC = gcc

CFLAGS += -Wall -Werror
CFLAGS += -g
CFLAGS += -I$(ROAD)

BINS = ClientUnix ServerUnix ClientTCP ServerTCP ServerUnixInet

all: $(BINS)

ClientUnix: ClientUnix.o socketFunction.o errorfunctions.o

ClientUnix.o: ClientUnix.c $(ROAD)socketFunction.h $(ROAD)errorfunctions.h

ServerUnix: ServerUnix.o socketFunction.o errorfunctions.o

ServerUnix.o: ServerUnix.c $(ROAD)socketFunction.h $(ROAD)errorfunctions.h

ClientTCP: ClientTCP.o socketFunction.o errorfunctions.o

ClientTCP.o: ClientTCP.c $(ROAD)socketFunction.h $(ROAD)errorfunctions.h

ServerTCP: ServerTCP.o socketFunction.o errorfunctions.o

ServerTCP.o: ServerTCP.c $(ROAD)socketFunction.h $(ROAD)errorfunctions.h

ServerUnixInet: ServerUnixInet.o socketFunction.o errorfunctions.o

ServerUnixInet.o: ServerUnixInet.c $(ROAD)socketFunction.h $(ROAD)errorfunctions.h

socketFunction.o: $(ROAD)socketFunction.c $(ROAD)socketFunction.h $(ROAD)errorfunctions.h
	$(CC) $(CFLAGS) -c -o socketFunction.o $(ROAD)socketFunction.c 

errorfunctions.o: $(ROAD)errorfunctions.c $(ROAD)errorfunctions.h
	$(CC) $(CFLAGS) -c -o errorfunctions.o $(ROAD)errorfunctions.c


clean:
	rm -f $(BINS) *.o SocketUNIX