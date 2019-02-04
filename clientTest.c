#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include "serialComms.h"
#include "client.h"

// extern int errno;
extern int enQueue(char *,char *,int );

void usage() {
    printf("\n\tUsage\n");

    printf("\t-d              Debug Mode\n");
    printf("\t-h|-?           Help\n");
    printf("\t-s <port name>  Set serial Port\n");
    printf("\t-v              Verbose\n");

    exit(0);
}

int main( int argc, char *argv[]) {
    int opt;
    int debug =  1;
    int verbose = 1;
    char *serialPort=(char *)NULL;
    int ser;
    char cmdBuffer[255];
    char *ptr;

    while ((opt = getopt(argc, argv, "dh?s:v")) != -1) {
        switch(opt) {
            case 'd':
                debug=1;
                break;
            case 'v':
                verbose=1;
                printf("\nVerbose mode on.\n");
                break;
            case 'h':
            case '?':
                usage();
                break;
            case 's':
                serialPort = (char *)malloc(strlen(optarg)+1);

                if( (char *)NULL == serialPort) {
                    perror("Failed to allocate memory for serial port");
                    exit(1);
                }
                strcpy(serialPort, optarg);
                break;
            default:
                break;
        }
    }

    if(verbose) {
        printf("\nSerial Port:%s\n",serialPort);
    }
    ser = open (serialPort, O_RDWR | O_NOCTTY | O_SYNC);

    if( 0 > ser) {
        printf("Failed to open serial port:%d\n",errno);
        perror("failed");
        exit(2);
    }

    setInterfaceAttribs (ser, B115200, 0);
    setBlocking (ser, 1);
    //
    // Need an alarm here
    //
    if ( redisPing(ser )) {
        printf("Redis UP\n");
    }
    printf("Q length=%d\n", enQueue(ser, (char *)"TEST", (char *)"ONE", 3));

    ptr=deQueue(ser,(char *)"TEST");
    printf(" Front >%s<\n", ptr);

    free(ptr);

    /*
       redisCommand(ser,(char *)cmdBuffer);
       strcpy(cmdBuffer,(char *)"LPUSH CHAN VALUE");
       redisCommand(ser,(char *)cmdBuffer);
     */

    //  strcpy(cmdBuffer,(char *)"LPOP CHAN");
    //  redisCommand(ser,(char *)cmdBuffer);


    //  enQueue((char *)"TEST",(char *)"data",4);
    return(0);
}
