
/* A simple server in the internet domain using TCP
 *   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <termios.h>
#include <pthread.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "rediSerial.h"
#include "strsave.h"
#include "serialComms.h"

globalSettings* globals;
#include<stdio.h>
#include<stdbool.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo) {
    if (signo == SIGHUP) {
        printf("received SIGHUP\n");
        globals->writeLog((char *)"received SIGHUP\n");
        globals->clrRunFlag();
    }
}


void error(const char *msg) {
    perror(msg);
    exit(1);
}

/*
 * Setup serial port and the block on read.
 */
void *serialThread(void *data) {

    char buf[2];     // Read one char at a time into this buffer.
    char inBuffer[32];
    char msgBuffer[32];

    int n;
    int i;

    /*
       if(globals->getDebug()) {
       printf("\nserialThread\n");
       }
       */
    //
    // set speed to 115,200 bps, 8n1 (no parity)
    //
    setInterfaceAttribs (globals->getSerial(), globals->getBaudRate(),0);
    //
    // set blocking
    //
    setBlocking (globals->getSerial(), 1);

    while (globals->getRunFlag() ) {
        n = read (globals->getSerial(), buf,1);
        /*
           if(globals->getDebug()) {
        //            putchar(buf[0]);
        fflush(stdout);
        }
        */

        if( buf[0] > 0x20 && buf[0] < 0x80 ) {
            sprintf( msgBuffer, "Out %02x:%c\n", buf[0],buf[0] );
        } else {
            sprintf( msgBuffer, "Out %02x:  \n", buf[0]);
        }
        globals->writeLog( msgBuffer );
        n = write(globals->getSocket(), buf, 1);
    }

    /* terminate the thread */
    globals->writeLog((char *)"Thread done\n");
    pthread_exit(NULL);
}

void usage() {
    printf("\nUsage\n");
    printf("\t-d\t\tDebug messages.\n");
    printf("\t-f\t\tRun in the foreground.\n");
    printf("\t-h|-?\t\tThis.\n");
    printf("\t-n <hostname|IP>\tAddress to listen on, default is localhost.\n");
    printf("\t-p <num>\tSet port number.  Default is 4001.\n");
    printf("\t-s <dev>\tSet serial port name. Default is /dev/ttyUSB0\n");
    printf("\t-v\t\tVerbose messages.\n");
    printf("\t\n");
    printf("\t\n");
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    char scratch[32];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int portNumber,opt;
    bool verbose=false;
    int tmp;
    int        thr_id;         // thread ID for the newly created thread 
    pthread_t  p_thread;       // thread's structure 
    int        a         = 1;  // thread 1 identifying number
    bool runFlag=true;
    //    bool exitFlag=false;
    bool daemon=false;
    pid_t process_id = 0;
    pid_t sid = 0;
    FILE *pidFile;

    strsave* serialPort;
    strsave* hostname;
    void **thrRet;

    globals = new globalSettings();

    portNumber = 4001;
    hostname = new strsave((char *)"localhost");
    serialPort = new strsave((char *)"/dev/ttyUSB0");


    while ((opt = getopt(argc, argv, "dfh?n:p:s:v")) != -1) {
        switch(opt) {
            case 'd':
                globals->setDebug();
                printf("\nDebug on\n");
                break;
            case 'f':
                daemon=false;
                break;
            case 'h':
            case '?':
                usage();
                exit(0);
                break;
            case 'n':
                if( hostname->get() != (char *)NULL ) {
                    delete hostname;
                }
                hostname = new strsave( optarg );
                break;
            case 'p':
                portNumber = atoi(optarg);
                break;
            case 's':
                if( serialPort->get() != (char *)NULL ) {
                    delete serialPort;
                }
                serialPort = new strsave( optarg );
                break;
            case 'v':
                verbose=true;
                printf("Verbose mode enabled\n");
                break;
            default:
                usage();
                exit(0);
                break;
        }
    }

    if( true == daemon) {
        globals->openLog((char *)"/var/log/serialServer.log");
    } else {
        globals->openLog((char *)"/dev/tty");
    }
    globals->writeLog((char *)"Logging info...\n");

    if(true == verbose) {
        printf ("Port:    %d\n",portNumber);
        printf ("Hostname:>%s<\n", hostname->get());

        printf ("Serial  :>%s<\n", serialPort->get());

        if( true == daemon) {
            printf("Run in background.\n");
        } else {
            printf("Run in foreground.\n");
        }
    }

    tmp = open (serialPort->get(), O_RDWR | O_NOCTTY | O_SYNC);

    globals->setBaudRate(B2400);

    //    globals->dump();

    if ( tmp < 0 ) {
        perror("Failed to open serial port");
        exit(2);
    }
    globals->setSerial( tmp );

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    memset((char*) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNumber);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        error("ERROR on binding");
    }


    pidFile=fopen((char *)"/var/run/serialServer.pid",(char *)"w");
    if( (FILE *)NULL == pidFile) {
        perror("PID FILE");
        exit(2);
    }
    fprintf(pidFile,"%d\n",getpid());
    fclose(pidFile);

    thr_id = pthread_create(&p_thread, NULL, serialThread, (void*)&a);

    while( globals->getRunFlag() ) {
        runFlag=true;

        while (runFlag) {
            bzero(buffer, sizeof(buffer));
            //            memset(buffer, 0, sizeof(buffer));
            //
            n = read(0, buffer, 3);

            if (n < 0) {
                error("ERROR reading from stdin");

                runFlag=false;
                globals->clrRunFlag();
                //                exitFlag = true;
            } else if (n>0) {
                if( buffer[0] > ' ' && buffer[0] <= 0x7f) {
                    sprintf(scratch,">%02x:%c<\n",buffer[0], buffer[0]);
                } else {
                    sprintf(scratch,">%02x<\n",buffer[0]);
                }
                globals->writeLog((char *)scratch);
                //                }
                // write char to serial port.
                n = write (globals->getSerial(), buffer,1);
        } else {
            runFlag = false;
        }
    }

    globals->writeLog((char *)"Waiting for thread");
    pthread_join( thr_id, thrRet);
    return 0;
}
}

