// Author: Dylan Barth
// Class: Vis147a, UCSD FA12
// Professor: Alicia Gibb
// Artist's Statement: This project is a blah blah blah 
// artist's statement.


/* ====================================================================
import statements
==================================================================== */


/* ====================================================================
Global Variables
==================================================================== */

// pins
int calibrationButton = 7;
int micPin = A0;

bool calibrated = false;

// const ints
int MY_SIZE = 199;


/* ====================================================================
Setup Loop
==================================================================== */
void setup() {
	Serial.begin(9600);
	pinMode(calibrationButton, INPUT);
	pinMode(micPin, INPUT);
}

/* ====================================================================
Void Loop
==================================================================== */

void loop() {
	// First time program starts, force a calibration
	if (calibrated == false) {
		int silenceThreshold = calibrate();
	}

	// with silence as baseline, take measurements 

		// read until something is over silence, (probably will have to play with these #'s) 

		// start appending, until you find silence again for a period of time

	// if average of the array meets time and volume threshold, initiate a response

	// randomResponse() will return a random canned response

	// speak(response) will speak the response
}

/* ====================================================================
Functions
==================================================================== */

int calibrate() {
	"""
	calibrate returns a int that represents the average level of noise in the surrounding environment. 
	The idea is to set a noise threshold so that human speech can be differentiated from the background. 
	Note: There are major flaws with this strategy =(
	"""
	// wait for user to press calibration button
	while (checkForSilence() == false) {
		continue;
	}

	// array of 'silent' input - sized at 200
	int silenceArray[MY_SIZE];
	int counter = 0;
	// record input while calibration button is pressed
	while (checkForSilence() == true) {
		int micInput = digitalRead(micPin);
		// only update array for first 200 values
		if (counter < MY_SIZE) {
			silenceArray = addToList(silenceArray, micInput);
		}
		counter ++;
	}
	// finally average array, return a silence threshold
	return int threshold = findAverage(silenceArray);
}

bool checkForSilence() {
	"""
	checkForSilence returns a boolean variable indicating whether or not the user 
	is holding down the calibration button. 
	"""
	// if digitalRead of calibrationButton returns 1, the user is not pressing.
	if (digitalRead(calibrationButton)) {
		return false;
	} else {
		return true;
	}
}

int addToList(list, input) {
	"""
	addToList takes two variables, an array and a value to be added. Finds the first 0
	space in the array and adds to value there, returning the new array.
	"""
	int counter = 0;
	int val = 1;
	// index through the array until it finds a 0 
	while (val != 0) {
		val = list[counter];
		counter ++;
	}
	// when it breaks, fill in this array value with desired value
	list[counter] = input;
	return list;
}

int findAverage(list) {
	"""
	findAverage take the list and returns the average value in the list.
	"""
	int counter = 0;
	int total = 0;
	while (counter < MY_SIZE) {
		total += list[counter];
	}
	return int avg = total / MY_SIZE;
}