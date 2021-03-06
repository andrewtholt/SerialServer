
CC?=g++
CFLAGS?=-g
CXX?=g++

BINS=serialServer tst clientTest # rediSerial

all:	$(BINS)

clientTest.o:	clientTest.cpp
	$(CC) -c $(CFLAGS) $? -o $@

clientTest:	clientTest.o client.o serialComms.o serialComms.h
	$(CC) $(CFLAGS) clientTest.o client.o serialComms.o -o $@


client.o:	client.cpp
	$(CC) $(CFLAGS) -c $? -o $@

rediSerial:	rediSerial.c rediSerial.h globals.o strsave.o serialComms.o
	$(CC) $(CFLAGS) $@.c globals.o strsave.o serialComms.o -o $@  -lpthread

serialServer:	serialServer.cpp rediSerial.h globals.o strsave.o serialComms.o 
	$(CXX) $(CFLAGS) $@.cpp globals.o strsave.o serialComms.o -o $@  -lpthread

serialComms.o:	serialComms.cpp
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


