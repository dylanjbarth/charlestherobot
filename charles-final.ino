/*
Dylan Barth
12/11/12
Piece: "Charles"
Artist's Statement:

This project poses one big question to the viewer. How can we really 
be sure that this reality, and our perception of it, truly is 
meaningful at all? In other words, what actually differentiates our 
experience and our perception of the world from Charles's? 

Charles follows the most basic rules of conversational flow, 
albeit roughly, by talking when you finish talking. He does not 
however even attempt to understand what you are saying, and tends to 
dominate the conversation with his own robotic ramblings. At times, 
he even playfully calls attention the obvious disconnect in the 
dialogue, stating "This conversation feels a bit one-sided, don't you 
think?" and "Sorry. I feel like my head is full of fluff today." 
which, of course, it is.

And yet despite his brusque and mechanical nature, it is difficult 
not to personify Charles. He has a name, a face, a charming British 
accent, and a personality, despite only knowing how to say 15 
different things. Clearly a simpleton, but at times, it isn't hard 
to imagine what type of person Charles might be, if given the 
computational bandwidth and exceptional equipment we humans have. 

We look at Charles, and with our impressive brains, sensory organs, 
complex networks of neurons and organic computational machinery, 
could hardly imagine living a life so limited as Charles does. Yet 
on our own scale, are we really so far removed from a microphone, 
a speaker, and a simple computational device directing the symphony?  

*/

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
#define FILECOUNT 18 // total .wavs

// variable ints
int timesAboveThreshold = 0; // a measure of number of intervals talking is true
int averageSilence = 0; // used to hold avg silence values
int measureAverage = 0; // used to hold mic sampling values
int response = 0;
int lastResponse = 0;


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

	// to shuffle the random generator, randomSeed with an unconnected pin
  randomSeed(analogRead(5));

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
	// pick a random response not the same as the previous 3
	while(true){
		response = random(3, FILECOUNT+1);
		if (response != lastResponse){
			lastResponse = response;
			break;
		}
	}
	Serial.println("Attention! Exited the while loop.");
	Serial.println(response);
	Serial.println("The above is the final pick.");
	// convert filename to char array
	String fileName = String(response) + ".wav";
	int tempLen = fileName.length()+1;
	char fileAsCharArray[tempLen];
	fileName.toCharArray(fileAsCharArray, tempLen);
	Serial.println(fileAsCharArray);
	// play random response
	delay(CONVOPAUSE);
	playcomplete(fileAsCharArray);
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