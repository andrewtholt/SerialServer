#include "rediSerial.h"
#include "strsave.h"

// using namespace std;


int main() {
    globalSettings* globals;
    strsave* str;

    globals = new globalSettings();

    globals->dump();

    str = new strsave((char *)"Hello");

    str->dump();

     printf("Result of equals is %d\n",str->equals((char *)"Hello"));
     printf("Result of equals is %d\n",str->equals((char *)"Helloooo"));

//    globals = new globalSettings;

    /*
    globals.setDebug();

    printf("Debug=%d\n",globals.getDebug());
    globals.clrDebug();
    printf("Debug=%d\n",globals.getDebug());
    */

    return(0);
}
