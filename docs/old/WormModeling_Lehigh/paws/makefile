CC=gcc
CFLAG= -c -O2 -w -g
LFLAG= -o

all: client server

client.o: client.c paws.h
	$(CC) $(CFLAG) client.c 

server.o: server.c paws.h
	$(CC) $(CFLAG) server.c

client: client.o paws.h
	$(CC) $(LFLAG) paws_client client.o -lm -O2

server: server.o paws.h
	$(CC) $(LFLAG) paws_server server.o -O2

clean:
	rm -rf *.o paws_client paws_server *~

