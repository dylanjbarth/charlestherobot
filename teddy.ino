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

bool silence = false;

int calibrationArray = new Array();


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
	// While user is calibrating, gather test data
	while (checkForSilence()) {

	}


		// after exiting while loop, average values into a 'calibrated silence int'

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

