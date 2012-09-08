/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 This example code is in the public domain.
 */

 #include "pitches.h"

// notes in the melody:
int melody[] = {
  //NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
  NOTE_A3, NOTE_A3, NOTE_G3, NOTE_A3, NOTE_G3, NOTE_F3, NOTE_G3, NOTE_E3};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  1, 1, 8, 4, 2, 4, 4, 2 };

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT); 
  pinMode(4, OUTPUT); 
  pinMode(5, OUTPUT); 
  pinMode(6, OUTPUT); 
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT); 
  pinMode(9, OUTPUT); 
  pinMode(11, OUTPUT);
  pinMode(12, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
    int buttonState = digitalRead(12);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    // turn LED on:    
    brew();
    //playDone();
    //playDone();
    //playDone();
  } 
}

void brew() {
  displayValue(4);
  delay(3000);
  
  displayValue(3);
  digitalWrite(9, HIGH);
  delay(5000);
  digitalWrite(9, LOW);
  delay(5000);
  
  displayValue(2);
  delay(5000);
  displayValue(1);
  delay(5000);
  displayValue(0);
  playDone();
  displayValue(-1);
}


void playDone() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8 ; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(11, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(11);
  }
}

void displayValue(int i)
{
  // turn off all LEDs
  for (int cnt = 2; cnt < 9; cnt++)
  {
    digitalWrite(cnt, LOW);
  }

  if(i==1)
  {
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 2)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
  }
  else if(i == 3)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 4)
  {
    digitalWrite(7, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 5)
  {
    digitalWrite(6, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 6)
  {
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 7)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 8)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 9)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(8, HIGH);
  }
  else if(i == 0)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
  }

}


