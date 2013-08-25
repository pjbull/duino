#include <Wire.h>
#include "CtrlM_funcs.h"

// ******************************************************************
// DIRECTIONAL TRIPWIRE - Peter Bull - November 2012
// 
// A pair of laser beams aimed at photoresistors are placed in a series
// in the direction of the motion to be tracked. Moving in one direction
// (i.e. breaking beam 1 before beam 2) triggers an event. Moving in the
// opposite direciton (i.e. breaking beam 2 before beam 1) performs a
// different action (or no action).
//
// CC BY-SA 3.0
//
// ******************************************************************

//================= PIN CONSTANTS
const int inPinOne = A0;  // Analog input pin that photoresistor 1 is attached to
const int inPinTwo = A1;  // Analog input pin that photoresistor 2 is attached to

//================= TIME CONSTANTS
const int LOOP_DELAY = 2;          // How long the main loop delays on each iteration (ms)
const int LED_DELAY = 10*1000;      // How long to keep the LED on after it has been triggered (ms); 2mins
const int RESET_DELAY = 5*1000;      // If the beams are tripped in the wrong direction, stop sensing for a period (ms)
const int SAMPLE_DELAY = 100;      // How often to sample the photoresistor value (ms)

//================= SENSOR CONSTANTS
const int DEV_MINIMUM = 50;       // Minimum deviance from the sensor baseline that will be counted as a trigger

//================= EVENT CONSTANTS
const int NORMALIZE_EVENT = 0;
const int LED_EVENT = 1;
const int RESET_EVENT = 2;
const int INITIAL_EVENT = 3;

//================= SENSOR VALUE STORAGE
int sensor1_value = 0;       // photoresistor value reading
int sensor1_base = NULL;     // initial sensor reading
int sensor1_deviation = 0;   // how much the value deviates from the base
int sensor1_normalTotal = 0; // total for normalizing the input

int sensor2_value = 0;       // photoresistor value reading
int sensor2_base = NULL;     // initial sensor reading     
int sensor2_deviation = 0;   // how much the value deviates from the base
int sensor2_normalTotal = 0; // total for normalizing the input

//================== GLOBAL TIME SOTRAGE
int normalCount = 0;
unsigned long time1 = NULL;
unsigned long time2 = NULL;
unsigned long time = 0;      
long diff = 0;

//================== BLINKM CONSTANTS
byte CTRLM_ADDR = 0x09;

//================== DEBUG
boolean DEBUG = false;

void setup() {
  if (DEBUG)
  {
    Serial.begin(9600);
  }
  
  setupCtrlM();
}

void setupCtrlM()
{
  Wire.begin();
  CtrlM_beginWithPower();
  delay(100); // wait a bit for things to stabilize
  CtrlM_off(CTRLM_ADDR);  // turn everyone off
}

void loop() {
  sensorLoop();
  
  // Use to debug ctrlm beahvior
  //debugSerialLoop();
}

void sensorLoop() {
  // read the analog in value:
  sensor1_value = analogRead(inPinOne);
  sensor2_value = analogRead(inPinTwo);
  
  // normalize sensor values if neither sensor has triggered
  if (((time1 == NULL && time2 == NULL) && time % SAMPLE_DELAY == 0) ||  // and the normSampleDelay has passed; or
      (sensor1_base == NULL && sensor2_base == NULL))                    // if hardware is reset, set base values
  {
    normalizeSensorBase();
  }

  // get the deviations from the base value
  sensor1_deviation = sensor1_value - sensor1_base;
  sensor2_deviation = sensor2_value - sensor2_base;
  
  // if the sensor is triggered and not already set.
  if (sensor1_deviation > DEV_MINIMUM && time1 == NULL)
  {
      time1 = time;
  }
  if (sensor2_deviation > DEV_MINIMUM && time2 == NULL)
  {
      time2 = time;
  }
  
  if (time1 != NULL && time2 != NULL)
  {
    diff = time2 - time1;
    
    // sensor1 triggered first
    if (diff < 0)
    {
      onTrigger();
    }
    
    // sensor 2 triggered first or simultaneous
    if (diff >= 0)
    {
        printEvent(RESET_EVENT);
        delay(RESET_DELAY); //maybe not necessary. needs testing.
    }
    
    time1 = NULL;
    time2 = NULL;
  }

  delay(LOOP_DELAY);  // short delay to let input settle
  time += LOOP_DELAY;
}

void onTrigger()
{
  // script 0 is the editable script in
  // the sequencer tool available here:
  // http://thingm.com/products/blinkm/quick-start-guide.html
  playScript(0); 
}

void simpleOnOff()
{
   printEvent(LED_EVENT);  
   Wire.beginTransmission(CTRLM_ADDR);
   Wire.write('c');
   Wire.write(0xff);
   Wire.write(0xff);
   Wire.write(0xff);
   Wire.endTransmission();
   delay(LED_DELAY);
   CtrlM_off(CTRLM_ADDR);
}

void playScript(byte scriptId)
{
  CtrlM_playScript(CTRLM_ADDR, scriptId, 0x04, 0x00);
  delay(LED_DELAY);
  CtrlM_off(CTRLM_ADDR);
}

// plays 5 seconds of each of the scripts
// on a blinkm.
void playScripts()
{
  for (byte b = 0; b < 0x11; b++)
  {
    CtrlM_playScript(CTRLM_ADDR, b, 0x04, 0x00);
    Serial.print("PLAYING SCRIPT NUMBER: "); 
    Serial.println(b);
    delay(5*1000);
    CtrlM_stopScript(CTRLM_ADDR);
  }
  CtrlM_off(CTRLM_ADDR);
}

void normalizeSensorBase()
{     
      // If the hardware is reset, give the base an initial value
      if (sensor1_base == NULL && sensor2_base == NULL)
      {
        sensor1_base = sensor1_value;
        sensor2_base = sensor2_value;
        printEvent(INITIAL_EVENT);       
        return;
      }

      // Add the sample values to the running total
      sensor1_normalTotal += sensor1_value;
      sensor2_normalTotal += sensor2_value;
      normalCount++;
      
      // Normalize base every 100 samplings
      if (normalCount == 100)
      {
        sensor1_base = sensor1_normalTotal / normalCount;
        sensor2_base = sensor2_normalTotal / normalCount;
        sensor1_normalTotal = 0;
        sensor2_normalTotal = 0;
        normalCount = 0;
        
        printEvent(NORMALIZE_EVENT);
      }
}

// debug CtrlM. To use:
// 1. switch loop method to debugSerialLoop()
// 2. upload to arduino
// 3. open serial monitor in Arduino  IDE (Tools > Serial Monitor)
// 4. type "1" and hit "Send" to test blinkm methods.
void debugSerialLoop(){
  Serial.println("READ:");
  int i = Serial.read();
  Serial.println(i, DEC);
  
  if(i==49)
  {
    Serial.println("TURN ON");
    playScript(0);
  }
  else
  {
    
  }
  delay(1000);
}

// prints certain events to the serial monitor
// used for debugging
void printEvent(int event)
{
  if (!DEBUG)
  {
    return;
  }
  
  switch (event) {
    case NORMALIZE_EVENT:
      Serial.println("NORMALIZE"); 
      Serial.print("s1_b = \t");      
      Serial.println(sensor1_base);
      Serial.print("s2_b = \t");      
      Serial.println(sensor2_base);
      break;
    case LED_EVENT:
      Serial.println("LED"); 
      Serial.print("diff = \t");      
      Serial.println(diff);
      break;
    case RESET_EVENT:
      Serial.println("RESET"); 
      Serial.print("diff = \t");      
      Serial.println(diff);
      break;
    case INITIAL_EVENT:
      Serial.println("INITIAL"); 
      Serial.print("s1_b = \t");      
      Serial.println(sensor1_base);
      Serial.print("s2_b = \t");      
      Serial.println(sensor2_base);
      break;
    default:
      break;
  }
  return;
}


