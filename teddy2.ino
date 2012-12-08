// Author: Dylan Barth
// Class: Vis147a, UCSD FA12
// Professor: Alicia Gibb
// Artist's Statement: This project is a blah blah blah 
// artist's statement.


/* ====================================================================
Wave Shield library and dependencies from: http://www.ladyada.net/make/waveshield/libraryhc.html
==================================================================== */

#include <WaveHC.h>
#include <WaveUtil.h>

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file 
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
#define FILE_COUNT 16 // number of WAV files on SD card

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

	// from WaveHC example code
	if (!card.init()) error("card.init");

	// enable optimized read - some cards may timeout
	card.partialBlockRead(true);

	if (!vol.init(card)) error("vol.init");

	if (!root.openRoot(vol)) error("openRoot");

	PgmPrintln("Index files");
	indexFiles();

	PgmPrintln("Play files by index");
	playByIndex();

	PgmPrintln("Play files by name");
	playByName();
}

/* ====================================================================
Void Loop
==================================================================== */

void loop() {
	// Part 1: Calibration
	if (calibrated == false) {
		// 0. wait for user to press button when things are quiet 
		while (digitalRead(calibrationButton) == 1) {
			// light LED?
			continue;
		}

		// 1. store a reading of values over time: MY_SIZE
		int counter = 0;
		int silenceArray[MY_SIZE];
		while (counter < MY_SIZE) {
			silenceArray[counter] = analogRead(micPin);
			counter ++;
		}

		// 2. find  & define silence average of these values
		int sum = 0;
		for (int i=0; i<MY_SIZE; i++) {
			sum += silenceArray[i];
		}
		averageSilence = sum / MY_SIZE;  
		calibrated = true;
	}


	// Part 2: Identifying Speech
	// A. Take measures until reading at least two talking measurements 
	do {
		// 0. create an array of measured values
		int counter = 0;
		int measureArray[MEASURE];
		while (counter < MEASURE) {
			measureArray[counter] = analogRead(micPin);
			counter ++;
		}

		// 1. average values
		int sum = 0;
		for (int i=0; i<MEASURE; i++) {
			sum += measureArray[i];
		}
		measureAverage = sum/MEASURE;

		// 2. compare this to silence threshold
		if (measureAverage > averageSilence) {
			timesAboveThreshold ++;
		}
		if (timesAboveThreshold > TALKING_MEASURES) {
			talking = true;
		}


	} while ((talking == false) || (measureAverage <= averageSilence)); // will exit loop when talking is true AND silence is achieved


	// Part 3: Responding
	// generate random number corresponding to response
	int randomNumber = random(1, 8);

	// use waveHC library

		// read until something is over silence, (probably will have to play with these #'s) 

		// start appending, until you find silence again for a period of time

	// if average of the array meets time and volume threshold, initiate a response

	// randomResponse() will return a random canned response

	// speak(response) will speak the response
}

/* ====================================================================
Functions
==================================================================== */

// from WaveHC example code:

// Files are 'touch tone phone' DTMF tones, P = #, S = *
// Most phones don't have A, B, C, and D tones.
// file names are of the form DTMFx.WAV where x is one of
// the letters from fileLetter[]
char fileLetter[] =  {'0', '1', '2', '3', '4', '5', '6', 
      '7', '8', '9', 'A', 'B', 'C', 'D', 'P', 'S'}; 
      
// index of DTMF files in the root directory
uint16_t fileIndex[FILE_COUNT];
/*
 * Find files and save file index.  A file's index is is the
 * index of it's directory entry in it's directory file. 
 */
void indexFiles(void) {
  char name[10];
  
  // copy flash string to RAM
  strcpy_P(name, PSTR("DTMFx.WAV"));
  
  for (uint8_t i = 0; i < FILE_COUNT; i++) {
    
    // Make file name
    name[4] = fileLetter[i];
    
    // Open file by name
    if (!file.open(root, name)) error("open by name");
    
    // Save file's index (byte offset of directory entry divided by entry size)
    // Current position is just after entry so subtract one.
    fileIndex[i] = root.readPosition()/32 - 1;   
  }
  PgmPrintln("Done");
}
/*
 * Play file by index and print latency in ms
 */
void playByIndex(void) {
  for (uint8_t i = 0; i < FILE_COUNT; i++) {
    
    // start time
    uint32_t t = millis();
    
    // open by index
    if (!file.open(root, fileIndex[i])) {
      error("open by index");
    }
    
    // create and play Wave
    if (!wave.create(file)) error("wave.create");
    wave.play();
    
    // print time to open file and start play
    Serial.println(millis() - t);
    
    // stop after PLAY_TIME ms
    while((millis() - t) < PLAY_TIME);
    wave.stop();
    
    // check for play errors
    sdErrorCheck();
  }
  PgmPrintln("Done");
}