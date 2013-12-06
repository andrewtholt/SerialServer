#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "strsave.h"

strsave::strsave(char *s) {
    char           *p;

    if ((p = (char *) malloc(strlen(s) + 1)) != NULL) {
        strcpy(p, s);
    }

    ptr=p;
}

strsave::~strsave() {
  if( (char *)NULL != ptr) {
    free(ptr);
  }
}

char* strsave::get() {
  return( ptr );
}

void strsave::dump() {
  if( (char *)NULL == ptr) {
    printf("Empty String\n");
  } else {
    printf("String is >%s<\n",ptr);
  }
}

bool strsave::equals( char *n) {

  if(!strcmp(n,ptr)) {
    return(1);
  } else {
    return(0);
  }

}