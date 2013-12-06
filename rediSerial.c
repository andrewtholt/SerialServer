/*
 *  thread.c
 *  
 *
 *  Created by Andrew Holt on 08/05/2008.
 *
 *  22 June 2013: TODO
 *
 *  Pass serial setting (speed, parity, word length etc.
 *  run as Daemon
 *
 */


#include <stdio.h>       /* standard I/O routines                 */
#include <pthread.h>     /* pthread functions and data structures */
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rediSerial.h"
#include "strsave.h"
#include "serialComms.h"

globalSettings* globals;

void usage() {
    printf("\nUsage\n");
    printf("\t-d\t\tDebug messages.\n");
    printf("\t-h|-?\t\tThis.\n");
    printf("\t-p <num>\tSet port number.  Default is 6379.\n");
    printf("\t-s <dev>\tSet serial port name. Default is /dev/ttyUSB0\n");
    printf("\t-v\t\tVerbose messages.\n");
    printf("\t\n");
    printf("\t\n");
}

/*
 * Setup serial port and the block on read.
 */
void *serialThread(void *data) {

    char buf[2];     // Read one char at a time into this buffer.
    int n;

    if(globals->getDebug()) {
        printf("\nserialThread\n");
    }
    //
    // set speed to 115,200 bps, 8n1 (no parity)
    //
    setInterfaceAttribs (globals->getSerial(), B115200, 0);
    //
    // set blocking
    //
    setBlocking (globals->getSerial(), 1);

    while (1) {
        n = read (globals->getSerial(), buf,1);

        if(globals->getDebug()) {
            putchar(buf[0]);
            fflush(stdout);
        }

        n = write(globals->getSocket(), buf, 1);
    }

    /* terminate the thread */
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int        thr_id;         /* thread ID for the newly created thread */
    pthread_t  p_thread;       /* thread's structure                     */
    int        a         = 1;  /* thread 1 identifying number            */
    int        b         = 2;  /* thread 2 identifying number            */

    int opt;
    int verbose = 0;

    int sockfd, portNumber, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    strsave* serialPort;
    strsave* hostname;
    char buffer[2];
    int tmp;

    hostname = new strsave((char *)"localhost");
    serialPort = new strsave((char *)"/dev/ttyUSB0");

    globals = new globalSettings();

    portNumber = 6379;

    while ((opt = getopt(argc, argv, "dh?p:s:v")) != -1) {
        switch(opt) {
            case 'd':
                globals->setDebug();
                printf("\nDebug on\n");
                break;
            case 'h':
            case '?':
                usage();
                exit(0);
                break;
            case 'p':
                portNumber = atoi(optarg);
                break;
            case 's':
                if( serialPort->get() != (char *)NULL ) {
		    delete serialPort;
//                    free(serialPort);
                }
                serialPort = new strsave( optarg );
                break;
            case 'v':
                verbose=1;
                printf("Verbose mode enabled\n");
                break;
            default:
                usage();
                exit(0);
                break;
        }
    }

    if(verbose) {
        printf ("Port:    %d\n",portNumber);
        printf ("Hostname:>%s<\n", hostname->get());

        printf ("Serial  :>%s<\n", serialPort->get());
    }


    tmp = open (serialPort->get(), O_RDWR | O_NOCTTY | O_SYNC);


    if ( tmp < 0 ) {
        perror("Failed to open serial port");
        exit(2);
    }
    globals->setSerial( tmp );

    tmp = socket(AF_INET, SOCK_STREAM, 0);
    if ( tmp < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    globals->setSocket(tmp);

    server = gethostbyname( hostname->get() );
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portNumber);

    if (connect( globals->getSocket(), (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(4);
    }

    if( (struct hostent *) NULL == server ) {
        perror("Opening socket:");
        exit(3);
    }


    if(globals->getVerbose()) {
        printf("Serial port FD  : %d\n", globals->getSerial());
        printf("Redid Socket FD : %d\n", globals->getSocket());
    }


    thr_id = pthread_create(&p_thread, NULL, serialThread, (void*)&a);

    while((n = read(globals->getSocket(),buffer,1)) > 0) {
        if( globals->getDebug()) {
            putchar(buffer[0]);
            fflush(stdout);
        }

        n = write(globals->getSerial(),buffer,1);
    }

    perror("What ?:");
    /* NOT REACHED */
    return 0;
}

