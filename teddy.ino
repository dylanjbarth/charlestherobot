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

int silenceButtonPin = 7;
bool silence = false;
int calibrationArray = new Array();
int calibrationLength = 32;

/* ====================================================================
Setup Loop
==================================================================== */
void setup() {
	Serial.begin(9600);
	pinMode(silenceButtonPin, INPUT);
}

/* ====================================================================
Void Loop
==================================================================== */

void loop() {
	// checkForSilence() for calibrationLength times;

	// while there is silence, append 
	Serial.println(digitalRead(silenceButtonPin));
}

/* ====================================================================
Functions
==================================================================== */



