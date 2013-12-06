#include <stdio.h>

class globalSettings {
    int sock;  // Redis socket
    int serial; // Serial port file descriptor;
    bool debug;
    bool verbose;
    bool daemon;
    FILE *out;
    int baudRate;
    bool runFlag;

    public:
        globalSettings();
        void clrRunFlag();
        bool getRunFlag();
        void setBaudRate( int br );
        int getBaudRate( );
        void dump();
        void setDebug();
        void clrDebug();
        int getDebug();
        void setVerbose();
        void clrVerbose();
        int getVerbose();
        void setSerial( int s);
        int getSerial();

        void setSocket(int r);
        int getSocket();
        bool openLog(char *name);
        void writeLog(char *msg);
};
