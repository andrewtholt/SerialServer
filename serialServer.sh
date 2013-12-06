#!/bin/sh

PIDFILE=/var/run/serialServer.pid
#
# Change ADDRESS to reflect real address.
#
ADDRESS="127.0.0.1"

PORT="4001"

FLAGS="-d -v -n ${ADDRESS} -p ${PORT}"

start() {
    # date >> /tmp/logfile.txt
    # echo $1 >> /tmp/logfile.txt

    if [ -f /var/run/serialServer.pid ]; then
        echo "Already running"
    else
        echo "Starting serialServer ...."
        /usr/local/bin/serialServer -d -v -n 192.168.0.143 -p 4001
        echo "... Started"
    fi
}

stop(){
    if [ -f /var/run/serialServer.pid ]; then
        PID=`cat /var/run/serialServer.pid`
        echo "Stopping serialServer ..."
        kill $PID
        rm /var/run/serialServer.pid
        echo "... Stopped"
    fi
}

case "$1" in
    start)
        start;;
    stop)
        stop;;
esac

