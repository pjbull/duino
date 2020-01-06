
// Analog pin 2
const int potPin = 2;

// Digital pins for LEDs
const int redLed = 5;
const int yellowLed = 6;
const int greenLed = 7;

// Digital pin for rangefinder
const int trigPin = 9;
const int echoPin = 10;

const int DISTANCE_BUFFER = 25; // +/- in centimeters the range for a good value

const float VARAINCE_LIMIT = 100;
const int HISTORY_LIMIT = 400;
int history[HISTORY_LIMIT];
int historyPosition = 0;
float histVariance = 0;

// defines variables
long duration;
int distance;
int limitDistance;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  
  Serial.begin(9600); // Starts the serial communication
}

void loop() {
  
  // get the distance from the sensor
  distance = readDistance();
//  Serial.println(distance);

  // get the boundary from the potentiometer
  limitDistance = readLimit();
//  Serial.println(limitDistance);

  // get the variance for the last set of observations
  histVariance = historyVariance(distance);

  // trigger the LEDs based on the results
  triggerLeds(distance, limitDistance, histVariance);
}

// read the actual distance from sensor
int readDistance() {
  int dist = 0;

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  dist = duration * 0.034 / 2;

  return dist;
}

// get the limit distance from the potentiometer
int readLimit() {
  // pot pin returns values between ~800 and ~1000, so
  // we rescale to ~0 - ~400 for distnace in CM
  return analogRead(potPin);
}

void triggerLeds(int actual, int limit, float var){
  if (var <= VARAINCE_LIMIT) {
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);

    return;
  }

  int closeDistance = max(0, limit - DISTANCE_BUFFER);
  int farDistance = min(500, limit + DISTANCE_BUFFER);

  if (actual < closeDistance) {
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  } else if ((closeDistance <= actual) && (actual <= farDistance)) {
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(greenLed, LOW);
  } else {
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, HIGH);
  }
}

float historyVariance(int newObs){
  // add new obs, looping to beginning as circular buff
  history[historyPosition++] = newObs;
  if (historyPosition == HISTORY_LIMIT - 1) historyPosition = 0;

  // calculate the sample mean
  int sum = 0;
  for (int i = 0; i < HISTORY_LIMIT; i++) {
    sum += history[i];
  }
  float avg = (float) sum / (float) HISTORY_LIMIT;

  // calculate the sample variance
  float diffs = 0.0;
  for (int i = 0; i < HISTORY_LIMIT; i++) {
    float hist_i = (float) history[i];
    diffs += (hist_i - avg) * (hist_i - avg);
  }

  // return variance
  float var = diffs / (float) (HISTORY_LIMIT - 1);

  Serial.println(var);

  return var;
}
