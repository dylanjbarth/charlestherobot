// Author: Dylan Barth
// Class: Vis147a, UCSD FA12
// Professor: Alicia Gibb
// Artist's Statement: This project is a blah blah blah 
// artist's statement.


/* ====================================================================
Wave Shield library and dependencies from: http://www.ladyada.net/make/waveshield/libraryhc.html
==================================================================== */

#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

/* ====================================================================
Global Variables
==================================================================== */

// pins
int micPin = A0;

// boolean flags
bool calibrated = false;
bool talking = false;
bool stillTalking;

// const ints
#define CALIBRATION_TIME 300 // number of readings for calibrating silence
#define error(msg) error_P(PSTR(msg))
#define BIGBUMPER 13 // for silence threshold
#define SMALLBUMPER 4 // for silence threshold
#define CONVOPAUSE 400 // in milliseconds

// variable ints
int timesAboveThreshold = 0; // a measure of number of intervals talking is true
int averageSilence = 0; // used to hold avg silence values
int measureAverage = 0; // used to hold mic sampling values
int lastResponse = 0; // used so that responses aren't repeated


/* ====================================================================
Setup Loop
==================================================================== */
void setup() {
	Serial.begin(9600);
	pinMode(micPin, INPUT);

	// ********* from WaveHC example code: *************************** //

	putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
	Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
	//  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
	if (!card.init()) {         //play with 8 MHz spi (default faster!)  
		putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
		sdErrorCheck();
		while(1);                            // then 'halt' - do nothing!
	}

	// enable optimize read - some cards may timeout. Disable if you're having problems
	// card.partialBlockRead(true);

	// Now we will look for a FAT partition!
	uint8_t part;
	for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
		if (vol.init(card, part)) 
	  break;                             // we found one, lets bail
	}
	if (part == 5) {                       // if we ended up not finding one  :(
		putstring_nl("No valid FAT partition!");
		sdErrorCheck();      // Something went wrong, lets print out why
		while(1);                            // then 'halt' - do nothing!
	}

	// Lets tell the user about what we found
	putstring("Using partition ");
	Serial.print(part, DEC);
	putstring(", type is FAT");
	Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?

	// Try to open the root directory
	if (!root.openRoot(vol)) {
		putstring_nl("Can't open root dir!"); // Something went wrong,
		while(1);                             // then 'halt' - do nothing!
	}

	// Whew! We got past the tough parts.
	putstring_nl("Ready!");

	// ********* end WaveHC example code: *************************** //

}

/* ====================================================================
Void Loop
==================================================================== */

void loop() {
	// Part 1: Calibration
	if (calibrated == false) {
		// 0. wait for Charles to tell us to calibrate.
		Serial.println("Loading...");
		delay(3000);
		playcomplete("1.wav");
		// 1. store a reading of values over time: CALIBRATION_TIME
		// but first an initial reading:
		averageSilence = analogRead(micPin);
		int counter = 0;
		while (counter <= CALIBRATION_TIME) {
			int temp = analogRead(micPin);
			averageSilence = (averageSilence + temp)/2; // averaging each time
			counter ++;
			Serial.print("avg silence is ");
			Serial.print(averageSilence);
			Serial.print(" in round ");
			Serial.println(counter);
		}
		Serial.println("End of calibration check.");
		Serial.print("Average silence is: ");
		Serial.println(averageSilence);

		calibrated = true;
		playcomplete("2.wav");
	}
	#define SILENCE averageSilence // set it constant for the rest of the program
	// #define SILENCE 340 // for speeding up debugging
	// Part 2. Identifying Speech 
	int volume = analogRead(micPin);
	// continue looping while volume is close to silence
	while ((volume < SILENCE+BIGBUMPER) && (volume > SILENCE-BIGBUMPER)) {
		volume = analogRead(micPin);
		Serial.print("Currently silent => Volume: ");
		Serial.println(volume);
		// will break when there is some noise
	}
	int avgVol = analogRead(micPin);
	Serial.print("You're talking! ======> Vol: ");
	Serial.println(avgVol);
	// after hearing noise, start averaging the volume
	do {
		while ((avgVol < SILENCE-SMALLBUMPER) || (avgVol > SILENCE+SMALLBUMPER)) {
			avgVol = (avgVol+analogRead(micPin))/2;
			Serial.print("Currently talking ==> Average volume: ");
			Serial.println(avgVol);
			// will break when it levels back out (ideally...)
		}
		Serial.print("I think you stopped talking, but I'm waiting for ");
		Serial.print(CONVOPAUSE);
		Serial.println(" seconds to check again.");
		delay(CONVOPAUSE);
		// check to see if they start talking again
		if ((analogRead(micPin) < SILENCE-SMALLBUMPER) || (analogRead(micPin) > SILENCE+SMALLBUMPER)) {
			stillTalking = true;
			avgVol = analogRead(micPin); // reset avgVol for when it reenters the loop
			Serial.print("I think you're still talking...");
		} else {
			stillTalking = false;
		}

	} while(stillTalking == true);
	
	// Part 3: Responding
	// pick a random response not the same as the previous
	while (true) {
		int temp = random(1, 9);
		if (temp != lastResponse) {
			lastResponse = temp;
			break;
		}
	}
	// play random response
	switch (lastResponse) { 
		case 1:
			playcomplete("3.wav");
			break;
		case 2:
			playcomplete("4.wav");
			break;
		case 3:
			playcomplete("5.wav");
			break;
		case 4:
			playcomplete("6.wav");
			break;
		case 5:
			playcomplete("7.wav");
			break;
		case 6:
			playcomplete("8.wav");
			break;
		case 7:
			playcomplete("9.wav");
			break;
		case 8:	
			playcomplete("10.wav");
			break;
		case 9:
			playcomplete("11.wav");
			break;
	}
	Serial.println("Reached the end of loop, restarting.");
	delay(CONVOPAUSE);
	// restart loop
}

/* ====================================================================
Functions
==================================================================== */

// ********* from WaveHC example code: *************************** //

/////////////////////////////////// HELPERS
/*
 * print error message and halt
 */
 void error_P(const char *str) {
 	PgmPrint("Error: ");
 	SerialPrint_P(str);
 	sdErrorCheck();
 	while(1);
 }
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
 void sdErrorCheck(void) {
 	if (!card.errorCode()) return;
 	PgmPrint("\r\nSD I/O error: ");
 	Serial.print(card.errorCode(), HEX);
 	PgmPrint(", ");
 	Serial.println(card.errorData(), HEX);
 	while(1);
 }

// Plays a full file from beginning to end with no pause.
 void playcomplete(char *name) {
  // call our helper to find and play this name
 	// Serial.println("before playfile");
 	// Serial.println(name);
 	playfile(name);
 	// Serial.println("after playfile");
 	while (wave.isplaying) {
  // do nothing while its playing
 	}
  // now its done playing
 }

 void playfile(char *name) {
 	// Serial.println("inside playfile()");
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
    // Serial.println("wave.stop()");
}
// Serial.println("past wave.isplaying check");
  // look in the root directory and open the file
if (!f.open(root, name)) {
	putstring("Couldn't open file "); Serial.print(name); return;
} 
// Serial.println("past cannot open file check");
  // OK read the file and turn it into a wave object
if (!wave.create(f)) {
	putstring_nl("Not a valid WAV"); return;
}
// Serial.println("past create wav file check");

  // ok time to play! start playback
wave.play();
// Serial.println("wave.play()");
}

// this handy function will return the number of bytes currently free in RAM, great for debugging!   
int freeRam(void)
{
	extern int  __bss_end; 
	extern int  *__brkval; 
	int free_memory; 
	if((int)__brkval == 0) {
		free_memory = ((int)&free_memory) - ((int)&__bss_end); 
	}
	else {
		free_memory = ((int)&free_memory) - ((int)__brkval); 
	}
	return free_memory; 
} 

// ********* end WaveHC example code: *************************** //


// old attempt at identifying Speech
// Part 2: Identifying Speech
	// A. Take measures until reading at least two talking measurements 
	// do {
	// 	// 0. take a sampling of the volume level in increments of MEASURE
	// 	int counter = 0;
	// 	while (counter < MEASURE) {
	// 		measureAverage = (measureAverage + analogRead(micPin))/2;
	// 		counter ++;
	// 		Serial.print("avg volume is ");
	// 		Serial.print(measureAverage);
	// 		Serial.print(" in round ");
	// 		Serial.println(counter);
	// 	}
	// 	Serial.println("End of measure check.");
	// 	Serial.print("Average volume is: ");
	// 	Serial.println(measureAverage);

	// 	// 1. compare this to silence threshold
	// 	if (measureAverage > SILENCE+10) {
	// 		Serial.println("Greater than silence!");
	// 		timesAboveThreshold ++;
	// 	}
	// 	if (timesAboveThreshold > TALKING_MEASURES) {
	// 		talking = true;
	// 	}
	// } while ((talking == false) || (measureAverage <= SILENCE)); // will exit loop when talking is true AND silence is achieved
	
	// // B. Reset measurement tools
	// timesAboveThreshold = 0; // reset counter
	// talking = false; // reset flag
	// measureAverage = analogRead(micPin); // reset measuring

	// // C. Debugging
	// Serial.print("I would be RESPONDING NOW! because: ");
	// if ((talking != false) && (measureAverage > SILENCE)){
	// 	Serial.println("talking != false and measureAvg is greater than SILENCE");
	// } else if (talking != false){
	// 	Serial.println("talking != false");
	// } else if (measureAverage > SILENCE){
	// 	Serial.println("measureAvg is greater than SILENCE");
	// } else {
	// 	Serial.println("Major problems are happening. give up now.");
	// }