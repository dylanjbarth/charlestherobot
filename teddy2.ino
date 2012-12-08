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
int calibrationButton = 7;
int micPin = A0;

// boolean flags
bool calibrated = false;
bool talking = false;

// const ints
#define MY_SIZE 500 // sample time length for calibrating silence
#define MEASURE 50 // sample time length for measuring talking
#define TALKING_MEASURES 2 // # of measures for talking threshold
#define FILE_COUNT 11 // number of WAV files on SD card
#define error(msg) error_P(PSTR(msg))

// variable ints
int timesAboveThreshold = 0; // a measure of number of intervals talking is true
int averageSilence = 0;
int measureAverage = 0;


/* ====================================================================
Setup Loop
==================================================================== */
void setup() {
	Serial.begin(9600);
	pinMode(calibrationButton, INPUT);
	pinMode(micPin, INPUT);

	putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
	Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
	//  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
	if (!card.init()) {         //play with 8 MHz spi (default faster!)  
	putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
	sdErrorCheck();
	while(1);                            // then 'halt' - do nothing!
	}

	// enable optimize read - some cards may timeout. Disable if you're having problems
	card.partialBlockRead(true);

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
}

/* ====================================================================
Void Loop
==================================================================== */

void loop() {
	// // Part 1: Calibration
	// if (calibrated == false) {
	// 	// 0. wait for user to press button when things are quiet 
	// 	while (digitalRead(calibrationButton) == 1) {
	// 		// light LED?
	// 		continue;
	// 	}

	// 	// 1. store a reading of values over time: MY_SIZE
	// 	int counter = 0;
	// 	int silenceArray[MY_SIZE];
	// 	while (counter < MY_SIZE) {
	// 		silenceArray[counter] = analogRead(micPin);
	// 		counter ++;
	// 	}

	// 	// 2. find  & define silence average of these values
	// 	int sum = 0;
	// 	for (int i=0; i<MY_SIZE; i++) {
	// 		sum += silenceArray[i];
	// 	}
	// 	averageSilence = sum / MY_SIZE;  
	// 	calibrated = true;
	// }


	// // Part 2: Identifying Speech
	// // A. Take measures until reading at least two talking measurements 
	// do {
	// 	// 0. create an array of measured values
	// 	int counter = 0;
	// 	int measureArray[MEASURE];
	// 	while (counter < MEASURE) {
	// 		measureArray[counter] = analogRead(micPin);
	// 		counter ++;
	// 	}

	// 	// 1. average values
	// 	int sum = 0;
	// 	for (int i=0; i<MEASURE; i++) {
	// 		sum += measureArray[i];
	// 	}
	// 	measureAverage = sum/MEASURE;

	// 	// 2. compare this to silence threshold
	// 	if (measureAverage > averageSilence) {
	// 		timesAboveThreshold ++;
	// 	}
	// 	if (timesAboveThreshold > TALKING_MEASURES) {
	// 		talking = true;
	// 	}


	// } while ((talking == false) || (measureAverage <= averageSilence)); // will exit loop when talking is true AND silence is achieved

	playcomplete("1.wav");
	playcomplete("2.wav");
	// Part 3: Responding
	switch (random(1, 9)) {
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


	// generate random number corresponding to aduio response file index
	// int randomFile = random(2, FILE_COUNT);
	// playByIndex(randomFile);
}

/* ====================================================================
Functions
==================================================================== */

// from WaveHC example code (chunks cut out for simplicity and function):

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
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
}

void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  
  // ok time to play! start playback
  wave.play();
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