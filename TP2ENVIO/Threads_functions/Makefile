ROAD = ../Modulos/
CC = gcc

CFLAGS += -Wall -Werror
CFLAGS += -g
CFLAGS += -I$(ROAD)

BINS = 2a 2b 3 4

all: $(BINS)

2a: 2a.o 

2a.o: 2a.c

2b: 2b.o

2b.o: 2b.c

3: 3.o 3_Fuctions.o

3.o: 3.c $(ROAD)3_Fuctions.h 

3_Fuctions.o: $(ROAD)3_Fuctions.c 
	$(CC) $(CFLAGS) -c -o 3_Fuctions.o $(ROAD)3_Fuctions.c 

4: 4.o 4_Functions.o matrix.o

4.o: 4.c $(ROAD)4_Functions.h 

matrix.o: $(ROAD)matrix.c $(ROAD)matrix.h
	$(CC) $(CFLAGS) -c -o matrix.o $(ROAD)matrix.c

4_Functions.o: $(ROAD)4_Functions.c $(ROAD)matrix.h
	$(CC) $(CFLAGS) -c -o 4_Functions.o $(ROAD)4_Functions.c 

clean:
	rm -f $(BINS) *.o 