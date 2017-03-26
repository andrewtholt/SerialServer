
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

globalSettings *globals;
#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler (int signo) {
    if (signo == SIGHUP) {
        printf ("received SIGHUP\n");
        globals->writeLog ((char *) "received SIGHUP\n");
        globals->clrRunFlag ();
    }
}


void error (const char *msg) {
    perror (msg);
    exit (1);
}

/*
 * Setup serial port and the block on read.
 * Reads from the serial and writes to the socket.
 */
void *serialThread(void *data) {

    char      buf[2];		// Read one char at a time into this buffer.
    char      inBuffer[32];
    char      msgBuffer[32];

    int       n;
    int       i;

    /*
     *       if(globals->getDebug()) {
     *       printf("\nserialThread\n");
     }
     */
    //
    // set speed to 115,200 bps, 8n1 (no parity)
    //
    setInterfaceAttribs (globals->getSerial (), globals->getBaudRate (), 0);
    //
    // set blocking
    //
    setBlocking (globals->getSerial (), 1);

    while (globals->getRunFlag ()) {
        n = read(globals->getSerial(), buf, 1);
        /*
         *           if(globals->getDebug()) {
         *        //            putchar(buf[0]);
         *        fflush(stdout);
         }
         */

        if (buf[0] > 0x20 && buf[0] < 0x80) {
            sprintf(msgBuffer, "Out %02x:%c\n", buf[0], buf[0]);
        }
        else {
            sprintf(msgBuffer, "Out %02x:  \n", buf[0]);
        }
        globals->writeLog (msgBuffer);
        n = write(globals->getSocket (), buf, 1);
    }

    /* terminate the thread */
    globals->writeLog ((char *) "Thread done\n");
    pthread_exit (NULL);
}

void usage () {
    printf ("\nUsage\n");
    printf ("\t-d\t\tDebug messages.\n");
    printf ("\t-f\t\tRun in the foreground.\n");
    printf ("\t-h|-?\t\tThis.\n");
    printf
        ("\t-n <hostname|IP>\tAddress to listen on, default is localhost.\n");
    printf ("\t-p <num>\tSet port number.  Default is 4001.\n");
    printf ("\t-s <dev>\tSet serial port name. Default is /dev/ttyUSB0\n");
    printf ("\t-b <baud rate>\tSet serial port speed\n");
    printf ("\t-v\t\tVerbose messages.\n");
    printf ("\n");
    printf ("\nNOTE: Default beheviour is:\n");
    printf ("\tserialServer -b 2400 -p 4001 -s /dev/ttyUSB0\n");
    printf ("\t\n");
    printf ("\t\n");
}

int main (int argc, char *argv[]) {
    int       sockfd, newsockfd, portno;
    socklen_t clilen;
    char      buffer[256];
    char      scratch[32];
    struct sockaddr_in serv_addr, cli_addr;
    int       n;
    int       portNumber, opt;
    bool      verbose = false;
    int       tmp;
    int       thr_id;		// thread ID for the newly created thread
    pthread_t p_thread;		// thread's structure
    int       a = 1;		// thread 1 identifying number
    bool      runFlag = true;
    //    bool exitFlag=false;
    bool      daemon = true;
    pid_t     process_id = 0;
    pid_t     sid = 0;
    FILE     *pidFile;

    strsave  *serialPort;
    strsave  *hostname;
    void    **thrRet;

    int       baudRate = 0;
    int       br = 0;

    globals = new globalSettings ();

    portNumber = 4001;
    hostname = new strsave ((char *) "localhost");
    serialPort = new strsave ((char *) "/dev/ttyUSB0");


    while ((opt = getopt (argc, argv, "b:dfh?n:p:s:v")) != -1) {
        switch (opt) {
            case 'b':
                baudRate = atoi (optarg);
                break;
            case 'd':
                globals->setDebug ();
                printf ("\nDebug on\n");
                break;
            case 'f':
                daemon = false;
                break;
            case 'h':
            case '?':
                usage ();
                exit (0);
                break;
            case 'n':
                if (hostname->get () != (char *) NULL) {
                    delete    hostname;
                }
                hostname = new strsave (optarg);
                break;
            case 'p':
                portNumber = atoi (optarg);
                break;
            case 's':
                if (serialPort->get () != (char *) NULL) {
                    delete    serialPort;
                }
                serialPort = new strsave (optarg);
                break;
            case 'v':
                verbose = true;
                printf ("Verbose mode enabled\n");
                break;
            default:
                usage ();
                exit (0);
                break;
        }
    }

    if (true == daemon) {
        globals->openLog ((char *) "/var/log/serialServer.log");
    }
    else {
        globals->openLog ((char *) "/dev/tty");
    }
    globals->writeLog((char *) "Logging info...\n");

    if (true == verbose) {
        printf ("Port:    %d\n", portNumber);
        printf ("Hostname:>%s<\n", hostname->get ());

        printf ("Serial  :>%s<\n", serialPort->get ());

        if (true == daemon) {
            printf ("Run in background.\n");
        }
        else {
            printf ("Run in foreground.\n");
        }
    }

    tmp = open(serialPort->get (), O_RDWR | O_NOCTTY | O_SYNC);

    switch (baudRate) {
        case 0:
        case 2400:
            br = B2400;
            break;
        case 4800:
            br = B4800;
            break;
        case 9600:
            br = B9600;
            break;
        case 19200:
            br = B19200;
            break;
        case 38400:
            br = B38400;
            break;
        case 57600:
            br = B57600;
            break;
        case 115200:
            br = B115200;
            break;
        default:
            br = B2400;
            break;
    }

    //    globals->setBaudRate(B2400);
    globals->setBaudRate (br);

    //    globals->dump();

    if (tmp < 0) {
        perror ("Failed to open serial port");
        exit (2);
    }
    globals->setSerial (tmp);

    sockfd = socket (AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error ("ERROR opening socket");
    }
    memset ((char *) &serv_addr, 0, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons (portNumber);

    if (bind (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        error ("ERROR on binding");
    }

    // Ready, do I stay in the foreground ?

    if (true == daemon) {
        process_id = fork ();
        // Indication of fork() failure
        if (process_id < 0) {
            printf ("fork failed!\n");
            // Return failure in exit status
            exit (1);
        }
        // PARENT PROCESS. Need to kill it.
        if (process_id > 0) {
            printf ("process_id of child process %d \n", process_id);
            // return success in exit status


            close (STDIN_FILENO);
            close (STDOUT_FILENO);
            close (STDERR_FILENO);

            exit (0);
        }
    }

    sid = setsid ();
    if (sid < 0) {
        exit (10);
    }


    pidFile = fopen ((char *) "/var/run/serialServer.pid", (char *) "w");
    if ((FILE *) NULL == pidFile) {
        perror ("PID FILE");
        exit (2);
    }
    fprintf (pidFile, "%d\n", getpid ());
    fclose (pidFile);

    thr_id = pthread_create(&p_thread, NULL, serialThread, (void *) &a);

    while (globals->getRunFlag ()) {
        runFlag = true;

        listen (sockfd, 5);
        clilen = sizeof (cli_addr);

        newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr, &clilen);
        globals->setSocket (newsockfd);

        if (newsockfd < 0) {
            error ("ERROR on accept");
        }

        while (runFlag) {
            memset (buffer, 0, sizeof (buffer));
            n = read (newsockfd, buffer, 1);

            if (n < 0) {
                error ("ERROR reading from socket");
                if (daemon) {
                    globals->writeLog ((char *) "ERROR reading from socket");
                }
                runFlag = false;
                globals->clrRunFlag ();
                //                exitFlag = true;
            } else if (n > 0) {
                if (buffer[0] > ' ' && buffer[0] <= 0x7f) {
                    sprintf (scratch, ">%02x:%c<\n", buffer[0], buffer[0]);
                } else {
                    sprintf (scratch, ">%02x<\n", buffer[0]);
                }
                globals->writeLog ((char *) scratch);

                // write char to serial port.
                n = write (globals->getSerial (), buffer, 1);
            }
            else {
                runFlag = false;
            }
        }

        close (newsockfd);
    }
    close (sockfd);

    globals->writeLog ((char *) "Waiting for thread");
    pthread_join (thr_id, thrRet);
    return 0;
}
