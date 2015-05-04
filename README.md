Serial Server

Connect to serial port via a socket.

Usage:
	-d		Debug messages.
	
	-f		Run in the foreground.
	
	-h|-?		This.
	
	-n <hostname|IP>	Address to listen on, default is localhost.
	
	-p <num>	Set port number.  Default is 4001.
	
	-s <dev>	Set serial port name. Default is /dev/ttyUSB0
	
	-b <baud rate>	Set serial port speed
	
	-v		Verbose messages.


NOTE: Default behaviour is:
	serialServer -b 2400 -p 4001 -s /dev/ttyUSB0
	
	
