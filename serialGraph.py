## Python script to import data from Arduino 
## and display results in graph

import serial	

# connect to arduino - make sure serial port is right
ser = serial.Serial('/dev/tty.usbserial', 9600)

# empty array to store session analog data
counter = 0
micVal = []

while True:
	micVal.append([counter, ser.readline()])
	counter ++ 