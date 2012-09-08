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
byte one[] =   {tr, br};
byte two[] =   {top, tr, mid, bl, bot};
byte three[] = {top, tr, mid, br, bot};
byte four[] =  {tl, tr, mid, br};
byte five[] =  {top, tl, mid, br, bot};
byte six[] =   {tl, mid, bl, br, bot};
byte seven[] = {top, tr, br};
byte eight[] = {top, tl, tr, mid, bl, br, bot};
byte nine[] =  {top, tl, tr, mid, br};
byte zero[] =  {top, tl, tr, bl, br, bot};

// The 7segment digit to display is equal to its index in this array
byte* digits[] = {zero, one, two, three, four, five, six, seven, eight, nine};

// Piezo output pin
byte piezoPin = 11;
#include "pitches.h"

// Bob Dylan's "One More Cup of Coffee" (the chorus, more or less...)
int melody[] = {NOTE_A3, NOTE_A3, NOTE_G3, NOTE_A3, NOTE_G3, NOTE_F3, NOTE_G3, NOTE_E3};
int noteDurations[] = {1, 1, 8, 4, 2, 4, 4, 2 };

// Other pins
byte startButtonPin = 12;   // Controls the momentary button which starts the brew process.
byte greenLedPin = 13;      // Contols the green brew finished LED
byte stirTransistorPin = 9; // Controls the transistor which gates the stirring mechanism

// Time intervals
int oneMinuteMs =  (1 *1000);
int halfMinuteMs = (3 *1000);

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the 7 segment display pins as output.
  for (byte ledPin = 0; ledPin < sizeof(digits[8]); ledPin++)
  {
    pinMode(ledPin, OUTPUT);
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
  
  // Stir for half a minute
  displayValue(3);
  digitalWrite(stirTransistorPin, HIGH);
  delay(halfMinuteMs);
  
  // Stop stirring
  digitalWrite(stirTransistorPin, LOW);
  delay(halfMinuteMs);

  // Countdown
  displayValue(2);
  delay(oneMinuteMs);
  displayValue(1);
  delay(oneMinuteMs);
  
  // Brew finished
  displayValue(0);
  playDone();
  digitalWrite(greenLedPin, HIGH);
  delay(oneMinuteMs);
  
  // turn off LEDs
  digitalWrite(greenLedPin, LOW);
  displayValue(-1);
}


void playDone() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(int)) ; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
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
  for (byte ledPin = 0; ledPin < sizeof(digits[8]); ledPin++)
  {
    digitalWrite(ledPin, LOW);
  }
  
  // if it is a single digit, set the appropriate pins to HIGH
  if(i > -1 && i < 10)
  {
    for (byte ledPin = 0; ledPin < sizeof(digits[i]); ledPin++)
    {
      digitalWrite(ledPin, HIGH);
    }
  }
  // else 7 segment display is off
}



