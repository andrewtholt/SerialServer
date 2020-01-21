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
#include <sys/poll.h>


#include <stdint.h>

#include "serialComms.h"
#include "client.h"

#define MS(n) ( n * 1000)

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

void dump(char *comment, uint8_t *ptr, int len) {

    printf("======\n");
    printf("%-16s:",comment);
    for(int i =0 ; i< len ; i++) {
        printf("%02x ", ptr[i]);
    }
    printf("\n");

    printf("%-16s:",comment);
    for(int i =0 ; i< len ; i++) {
        if( ptr[i] > 0x1f && ptr[i] < 0x80) {
            printf(" %c ", ptr[i]);
        } else {
            printf(" . ");
        }
    }

    printf("\n======\n");
}

void flushRx( int ser ) {
    struct pollfd fds[2];
    uint8_t byte;
    int n;
    int ret;

    do {
        ret = poll(fds,1, 10);
        fds[0].fd = ser;
        fds[0].events = POLLIN;
        n = read(ser, &byte, 1);
        printf("%d bytes read\n",n);
    } while(ret <= 0) ;
}

int sendCmd( int ser,uint8_t *command, uint8_t *response,int len) {
    int count=1;
    int n ;
    int ret;
    struct pollfd fds[2];
    uint8_t tmp[2];
    int r;

    dump((char *)"Command",command,len);

    memset( response, 0, 255);
    memset( tmp, 0, sizeof(tmp));
    do {
        printf("%02d\n", count++);
        fds[0].fd = ser;
        fds[0].events = POLLIN;
        n = write(ser, command, 7);

        ret = poll(fds,1, 1000);
    } while(ret <= 0) ;
    r = read(ser, tmp, 1 );

    dump((char *)"Response",tmp,2);

    tmp[0] = 0x05; // ENQ
    n = write(ser, tmp, 1);

    r = read(ser, response, 255);
    uint16_t l = ( response[1] << 8) + ( response[2] );
    printf("==>len=%02d\n", l);
    r = read(ser, response+r, 255);

    dump((char *)"Sent ENQ", response, 24);
    return r;
}

int cardReset( int ser ) {
    uint8_t cmd[16];
    uint8_t response[255];

    memset( cmd, 0, 16);
    cmd[0]=0x02;
    cmd[1]=0x00;
    cmd[2]=0x02;
    cmd[3]=0x30;
    cmd[4]=0x31;
    //    cmd[4]=0x30;
    cmd[5]=0x03;

    cmd[6] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5];

    return(sendCmd( ser,cmd,response, 7));
}

int checkStatus( int ser ) {
    uint8_t cmd[16];
    uint8_t response[255];

    memset( cmd, 0, 16);
    cmd[0]=0x02;
    cmd[1]=0x00;
    cmd[2]=0x02;
    cmd[3]=0x31;
    cmd[4]=0x30;
    cmd[5]=0x03;

    cmd[6] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5];

    return(sendCmd( ser,cmd,response, 7));
}

int cardEject( int ser ) {
    uint8_t cmd[16];
    uint8_t response[255];

    memset( cmd, 0, 16);
    cmd[0]=0x02;
    cmd[1]=0x00;
    cmd[2]=0x02;
    cmd[3]=0x32;
    cmd[4]=0x30;
    cmd[5]=0x03;

    cmd[6] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5];

    return(sendCmd( ser,cmd,response, 7));
}



int main( int argc, char *argv[]) {
    int opt;
    int debug =  1;
    int verbose = 1;
    char *serialPort=(char *)NULL;
    int ser;
    char cmdBuffer[255];
    char *ptr;

    struct pollfd fds[2];

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


    setInterfaceAttribs (ser, B9600, 0);
    setBlocking (ser, 1);

    int r ;
    int count = 1;
    int ret=-1;

//    r=cardReset( ser );
    r=cardEject( ser );

    exit(0);

    for( int i=0; i<10;i++) {
//        flushRx( ser );
        usleep(MS(1000));

        printf("around %02d and ...\n", i);
        r=cardReset( ser );
        printf("cardReset ret=%02d\n",r);

        /*
           r= checkStatus( ser );
           printf("checkStatus ret=%02d\n",r);
         */
    }

    return(0);
}
