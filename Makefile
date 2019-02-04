
CC=g++
CFLAGS=-g

BINS=serialServer rediSerial tst clientTest

all:	$(BINS)

clientTest.o:	clientTest.c
	$(CC) -c $(CFLAGS) $? -o $@

clientTest:	clientTest.o client.o serialComms.o serialComms.h
	$(CC) $(CFLAGS) clientTest.o client.o serialComms.o -o $@


client.o:	client.c
	$(CC) $(CFLAGS) -c $? -o $@

rediSerial:	rediSerial.c rediSerial.h globals.o strsave.o serialComms.o
	$(CC) $(CFLAGS) $@.c globals.o strsave.o serialComms.o -o $@  -lpthread

serialServer:	serialServer.cpp rediSerial.h globals.o strsave.o serialComms.o 
	$(CXX) $(CFLAGS) $@.cpp globals.o strsave.o serialComms.o -o $@  -lpthread

serialComms.o:	serialComms.c
	$(CC) -c $(CLAGS) $? -o $@

tst:	tst.cpp globals.o strsave.o
	$(CC) $(CFLAGS) $@.cpp globals.o strsave.o -o $@

globals.o:	globals.cpp rediSerial.h
	$(CC) $(CLAGS) -c globals.cpp -o $@

strsave.o:	strsave.cpp strsave.h
	$(CC) $(CLAGS) -c strsave.cpp -o $@

install:
	cp ./serialServer $(DESTDIR)/usr/local/bin
clean:
	rm -f $(BINS) *.o cscope.out

backup:	clean
	./backup.sh


