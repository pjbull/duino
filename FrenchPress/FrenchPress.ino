/*
  FrenchPress
 Controls an automatic French Press coffee maker. Stirs after first minute,
 counts down time, and plays a tune a brew completion.
 
 See http://peterbull.me/tt_fp.html for details on the construction
 and Fritzing circuit diagram.
 
 Prereqs:
 Add 205 F water and freshly ground coffee beans. Press start button.
 
 This code performs the following steps:
 1. Set 7segment display to 4. 
 2. Wait one minute.
 3. Set 7 segment display to 3.
 3. Begin stirring coffee by triggering magnetic stirrer mechanism.
 4. Stir for 30s.
 5. Wait 30s.
 6. Set 7 segment display to 2.
 7. Wait one minute.
 8. Set 7 segment display to 1.
 9. Wait one minute.
 10. Set 7 segment display to 0.
 11. Play brew complete melody.
 12. Turn on green brew comeplete LED.
 13. Wait 1 minute.
 14. Turn off LEDs.
 
 Postreqs:
 Press french press. Pour coffee. Enjoy a masterful brew.
 
 (CC BY-SA 3.0) Peter Bull
 */

// 7 segment LED to pin mapping
byte top = 2; //      top
byte tl = 3;  //      ---
byte tr = 4;  //  tl |   | tr
byte mid = 5; //  mid ---  
byte bl = 6;  //  bl |   | br
byte br = 7;  //      ---
byte bot = 8; //      bot

// Tells the arduino which pins to turn on for which digits.
// For convenience, the number of LEDs to light up is the first entry, which
// is a little cheaper than a struct or listing all the pins in each array.
byte one[] =   {2, tr, br};
byte two[] =   {5, top, tr, mid, bl, bot};
byte three[] = {5, top, tr, mid, br, bot};
byte four[] =  {4, tl, tr, mid, br};
byte five[] =  {5, top, tl, mid, br, bot};
byte six[] =   {5, tl, mid, bl, br, bot};
byte seven[] = {3, top, tr, br};
byte eight[] = {7, top, tl, tr, mid, bl, br, bot};
byte nine[] =  {5, top, tl, tr, mid, br};
byte zero[] =  {6, top, tl, tr, bl, br, bot};

// The 7segment digit to display is equal to its index in this array
byte* digits[] = {zero, one, two, three, four, five, six, seven, eight, nine};

// Piezo output pin
byte piezoPin = 11;
#include "pitches.h"

// The final countdown . . . to coffee!
int melody[] = {
  NOTE_FS5, NOTE_E5, NOTE_FS5, NOTE_B4,
  NOTE_G5, NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_E5,
  NOTE_G5, NOTE_FS5, NOTE_G5, NOTE_B4,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_CS5, NOTE_E5, NOTE_D5,
  
  NOTE_FS5, NOTE_E5, NOTE_FS5, NOTE_B4,
  NOTE_G5, NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_E5,
  NOTE_G5, NOTE_FS5, NOTE_G5, NOTE_B4,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_CS5, NOTE_E5, NOTE_D5,
  
  NOTE_CS5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_E5, NOTE_FS5, NOTE_E5, NOTE_D5, NOTE_CS5,
  NOTE_B4, NOTE_G5, NOTE_FS5,
  NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_E5,
  
  NOTE_FS5, NOTE_FS5, NOTE_FS5
};

int noteDurations[] = {
  8, 8, 2, 1,
  8, 8, 4, 4, 2,
  8, 8, 2, 1,
  8, 8, 4, 4, 4, 4, 2,
  
  8, 8, 2, 1,
  8, 8, 4, 4, 2,
  8, 8, 2, 1,
  8, 8, 4, 4, 4, 4, 2,
  
  8, 8, 2,
  8, 8, 4, 4, 4, 4,
  2, 2, 1,
  8, 8, 8, 8,
  13 //special logic to extend last note
};

// Other pins
byte startButtonPin = 12;   // Controls the momentary button which starts the brew process.
byte greenLedPin = 13;      // Contols the green brew finished LED
byte stirTransistorPin = 9; // Controls the transistor which gates the stirring mechanism

// Time intervals
unsigned long oneMinuteMs =  60L * 1000L;
unsigned long twentySecsMs = 20L * 1000L;
unsigned long fortySecsMs =  40L * 1000L;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the 7 segment display pins as output.
  for (byte i = top; i <= bot; i++)
  {
    pinMode(i, OUTPUT);
  }
  
  // initialize the other pins
  pinMode(piezoPin, OUTPUT);
  pinMode(startButtonPin, INPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(stirTransistorPin, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  int buttonState = digitalRead(startButtonPin);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {       
    brew();
  } 
}

void brew() {
  // Start countdown
  displayValue(4);
  delay(oneMinuteMs);
  
  // Stir for 20s
  displayValue(3);
  digitalWrite(stirTransistorPin, HIGH);
  delay(twentySecsMs);
  
  // Stop stirring
  digitalWrite(stirTransistorPin, LOW);
  delay(fortySecsMs);

  // Countdown
  displayValue(2);
  delay(oneMinuteMs);
  displayValue(1);
  delay(oneMinuteMs);
  
  // Brew finished
  displayValue(0);
  digitalWrite(greenLedPin, HIGH);
  playDone();
  delay(oneMinuteMs);
  
  // turn off LEDs
  digitalWrite(greenLedPin, LOW);
  displayValue(-1);
}


void playDone() {
  for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(int)) ; thisNote++) {   
    int noteDuration = noteDurations[thisNote] != 13 ? 1000/noteDurations[thisNote] : 2500;
    tone(piezoPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(piezoPin);
  }
}

void displayValue(int i)
{
  // turn off all LEDs by setting pins in the digit 8 to LOW
  for (byte cnt = 1; cnt <= digits[8][0]; cnt++)
  {
    digitalWrite(digits[8][cnt], LOW);
  }
  
  // if it is a single digit, set the appropriate pins to HIGH
  if(i > -1 && i < 10)
  {
    for (byte cnt = 1; cnt <= digits[i][0]; cnt++)
    {
      digitalWrite(digits[i][cnt], HIGH);
    }
  }
  // else 7 segment display is off
}



