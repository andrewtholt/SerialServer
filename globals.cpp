#include "rediSerial.h"
#include <unistd.h>
#include <stdlib.h>

globalSettings::globalSettings() {
   sock=0;
   serial=0;
   debug=0;
   baudRate=0;
   out=(FILE *)NULL;
   runFlag=true;
}

void globalSettings::setBaudRate(int br) {
    baudRate=br;
}

void globalSettings::clrRunFlag() {
    runFlag=false;
}

bool globalSettings::getRunFlag() {
    return(runFlag);
}

int globalSettings::getBaudRate() {
    return baudRate;
}
void globalSettings::setDebug() {
  debug=1;
  printf("\nDebug mode Set\n");
}

void globalSettings::dump() {
  printf("\n\tDump\n");
  printf("\nBaud Rate : %d", baudRate);
  printf("\n");
}
void globalSettings::clrDebug() {
  debug=0;
  printf("\nDebug mode Clear\n");
}
int globalSettings::getDebug() {
  return(debug);
}
void globalSettings::setVerbose() {
  verbose=1;
  printf("\nVerbose mode Set\n");
}
void globalSettings::clrVerbose() {
  verbose=0;
  printf("\nVerbose mode Clear\n");
}
int globalSettings::getVerbose() {
  return(verbose);
}


void globalSettings::setSerial( int s) {
  serial=s;
}

int globalSettings::getSerial() {
  return(serial);
}

void globalSettings::setSocket(int r) {
  sock=r;
}

int globalSettings::getSocket() {
  return(sock);
}

bool globalSettings::openLog(char *name) {

    out=fopen(name,"a");

    if( (FILE *)NULL == out) {
        fprintf(stderr,"Failed to open output file %s\n", name);
        exit(-1);
    }
}

void globalSettings::writeLog(char *msg) {
    if ( out != (FILE *)NULL) {
        fprintf(out,msg);
        fflush( out );
    }
}
