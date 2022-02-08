
#include "avdweb_VirtualDelay.h"
#include <ButtonDebounce.h>
#include <Streaming.h>

#define SPACE 0
#define MIND 1
#define SOUL 2
#define REALITY 3
#define TIME 4
#define POWER 5

#define TRIANGLE 0
#define CIRCLE 1
#define GAUSS 2

int ledPins[] = {3, 5, 6, 9, 10, 11};

ButtonDebounce button14(14, 250);
ButtonDebounce button15(15, 250);
ButtonDebounce button16(16, 250);
ButtonDebounce button17(17, 250);
ButtonDebounce button18(18, 250);


float inhale = 0, inhale_retention = 0, exhale = 0, exhale_retention = 0;
int stones[2];
VirtualDelay delayInhale, delayInhaleHold, delayExhale, delayExhaleHold, delayBlink;

float smoothness = 300; //larger=slower change in brightness 
float gamma = 0.14; // affects the width of peak (more or less darkness)
float beta = 0.5; // shifts the gaussian to be symmetric

void setup() {
  Serial.begin(9600);

  for (int index = 0; index < sizeof(ledPins); index++) {
    pinMode(ledPins[index], OUTPUT);
  }
  
  randomSeed(analogRead(0));
  turnLEDSOff();
  pingPong();
}

void loop() {
  checkInput();
  
  if (inhale) {
    sinePulseNonBlocking(stones, inhale, inhale_retention, exhale, exhale_retention);
  }
}

void sinePulseNonBlocking(int stones[], float inhale_speed, float inhale_retention, float exhale_speed, float exhale_retention) {
  static float in = 4.72;
  float out;

  int multiplier = in > 7.85 ? exhale_speed : inhale_speed;
  in = in + (multiplier * 0.0005);

  // reset from the top
  if (in > 10.995) {
    in = 4.712;
  }

  // bottom
  if (in > 4.71 && in < 4.72) {
    delayExhaleHold.start(exhale_retention);
    if (delayExhaleHold.elapsed()) {
      in = 4.72;
    }
    else {
      in = 4.55;
    }
  }

  // top
  if (in > 7.85 && in < 7.86) {
    delayInhaleHold.start(inhale_retention);
    in = delayInhaleHold.elapsed() ? 7.86 : 7.0;
  }
  
  out = sin(in) * 127.5 + 127.5;
  turnLEDS(stones, out);
}

void checkInput() {
  button14.update();
  button15.update();
  button16.update();
  button17.update();
  button18.update();

  if (button14.state() == LOW) {
    randomEffect();
    // 4-4-4  Triangle Breathing, reduce metabolic rate
    stones[0] = random(0, 5);
    stones[1] = random(0, 5);
    inhale = 2.5;
    inhale_retention = 3500;
    exhale = 2.5;
    exhale_retention = 3500;
  }
  else if (button15.state() == LOW) {
    randomEffect();
    // 1-1-1-1  Square Breathing, preserve energy
    stones[0] = random(0, 5);
    stones[1] = random(0, 5);
    inhale = 4;
    inhale_retention = 1000;
    exhale = 4;
    exhale_retention = 1000;
  }
  else if (button16.state() == LOW) {
    randomEffect();
    // 1-2  Paced Breathing, stress reduction
    stones[0] = random(0, 5);
    stones[1] = random(0, 5);
    inhale = 4;
    inhale_retention = 1000;
    exhale = 2.5;
    exhale_retention = 500;
  }
  else if (button17.state() == LOW) {
    randomEffect();
    // 4-7-8 Sleeping. extreme relaxation
    stones[0] = random(0, 5);
    stones[1] = random(0, 5);
    inhale = 2.5;
    inhale_retention = 6500;
    exhale = 1;
    exhale_retention = 7500;
  }
  else if (button18.state() == LOW) {
    randomEffect();
    // 1-2-1 Awake. Energizing
    stones[0] = random(0, 5);
    stones[1] = random(0, 5);
    inhale = 6;
    inhale_retention = 200;
    exhale = 6;
    exhale_retention = 100;
  }
}

void sinePulse(int stones[], float inhale_speed, float inhale_retention, float exhale_speed, float exhale_retention) {
  float in, out;

  // rising
  out = sinePulseRise(stones, inhale_speed);

  // hold
  blinkLEDS(stones, out);
  delayInhaleHold.start(inhale_retention);
  while (!delayInhaleHold.elapsed()) {};
  blinkLEDS(stones, out);

  // falling
  out = sinePulseFall(stones, exhale_speed);

  // hold
  delayExhaleHold.start(exhale_retention);
  while (!delayExhaleHold.elapsed()) {};
}

int sinePulseRise(int stones[], float inhale_speed) {
  float in, out;
  
  for (in = 4.712; in < 7.854; in = in + (inhale_speed * 0.0005)) {
      delayInhale.start(1); 
      while (!delayInhale.elapsed()) {};
      
      out = sin(in) * 127.5 + 127.5;
      turnLEDS(stones, out);
  }

  return out;
}


int sinePulseFall(int stones[], float exhale_speed) {
  float in, out;

  for (in = 1.570; in < 4.712; in = in + (exhale_speed * 0.0005)) {
      delayExhale.start(1); 
      while (!delayExhale.elapsed()) {};
      
      out = sin(in) * 127.5 + 127.5;
      turnLEDS(stones, out);
  }
}

void turnLEDS(int stones[], int out) {
  for (int i = 0; i < sizeof(stones); i++) {
    analogWrite(ledPins[stones[i]], out);
  }
}


void turnLEDSOn() {
  for (int i = 0; i < sizeof(ledPins); i++) {
    analogWrite(ledPins[i], 255);
  }
}

void turnLEDSOff() {
  for (int i = 0; i < sizeof(ledPins); i++) {
    analogWrite(ledPins[i], 0);
  }
  
  inhale = 0;
  inhale_retention = 0;
  exhale = 0;
  exhale_retention = 0;
}

void blinkLEDS(int stones[], int out) {
  turnLEDS(stones, out-100);
  delay(100);
  turnLEDS(stones, out-150);
  delay(100);
  turnLEDS(stones, out);
}

void randomEffect() {
  turnLEDSOff();
  
  int k = random(0, 5);

  Serial << "\n k: " << k;

  switch (k) {
    case 0:
      oneAfterAnotherNoLoop();
      break;
    case 1:
      oneAfterAnotherLoop();
      break;
    case 2:
      oneOnAtATime();
      break;
    case 3:
      marquee();
      break;
    default:
      randomLED();
      break;
  }
}

void oneAfterAnotherNoLoop() {
  // time (milliseconds) to pause between LEDs
  int delayTime = 100;

  // turn all the LEDs on:

  digitalWrite(ledPins[0], HIGH);  //Turns on LED #0 (pin 4)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[1], HIGH);  //Turns on LED #1 (pin 5)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[2], HIGH);  //Turns on LED #2 (pin 6)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[3], HIGH);  //Turns on LED #3 (pin 7)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[4], HIGH);  //Turns on LED #4 (pin 8)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[5], HIGH);  //Turns on LED #5 (pin 9)
  delay(delayTime);                //wait delayTime milliseconds

  // turn all the LEDs off:

  digitalWrite(ledPins[5], LOW);   //Turn off LED #5 (pin 9)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[4], LOW);   //Turn off LED #4 (pin 8)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[3], LOW);   //Turn off LED #3 (pin 7)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[2], LOW);   //Turn off LED #2 (pin 6)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[1], LOW);   //Turn off LED #1 (pin 5)
  delay(delayTime);                //wait delayTime milliseconds
  digitalWrite(ledPins[0], LOW);   //Turn off LED #0 (pin 4)
  delay(delayTime);                //wait delayTime milliseconds
}

void oneAfterAnotherLoop() {
  int index;
  int delayTime = 100; // milliseconds to pause between LEDs
  // make this smaller for faster switching

  for (index = 0; index <= 5; index++) {
    digitalWrite(ledPins[index], HIGH);
    delay(delayTime);
  }

  // Turn all the LEDs off:

  for (index = 5; index >= 0; index--) {
    digitalWrite(ledPins[index], LOW);
    delay(delayTime);
  }
}
void oneOnAtATime() {
  int index;
  int delayTime = 100; // milliseconds to pause between LEDs
  // make this smaller for faster switching

  // step through the LEDs, from 0 to 5

  for (index = 0; index <= 5; index++) {
    digitalWrite(ledPins[index], HIGH);  // turn LED on
    delay(delayTime);                    // pause to slow down
    digitalWrite(ledPins[index], LOW);   // turn LED off
  }
}

void pingPong() {
  int index;
  int delayTime = 100; // milliseconds to pause between LEDs
  // make this smaller for faster switching

  // step through the LEDs, from 0 to 5

  for (index = 0; index <= 5; index++) {
    digitalWrite(ledPins[index], HIGH);  // turn LED on
    delay(delayTime);                    // pause to slow down
    digitalWrite(ledPins[index], LOW);   // turn LED off
  }

  // step through the LEDs, from 5 to 0

  for (index = 5; index >= 0; index--) {
    digitalWrite(ledPins[index], HIGH);  // turn LED on
    delay(delayTime);                    // pause to slow down
    digitalWrite(ledPins[index], LOW);   // turn LED off
  }
}

void marquee() {
  int index;
  int delayTime = 200; // milliseconds to pause between LEDs
  // Make this smaller for faster switching

  for (index = 0; index <= 2; index++) {
    digitalWrite(ledPins[index], HIGH);    // Turn a LED on
    digitalWrite(ledPins[index + 3], HIGH); // Skip four, and turn that LED on
    delay(delayTime);                      // Pause to slow down the sequence
    digitalWrite(ledPins[index], LOW);     // Turn the LED off
    digitalWrite(ledPins[index + 3], LOW); // Skip four, and turn that LED off
  }
}

void randomLED() {
  int index;
  int delayTime;

  index = random(5);    // pick a random number between 0 and 5
  delayTime = 100;

  digitalWrite(ledPins[index], HIGH);  // turn LED on
  delay(delayTime);                    // pause to slow down
  digitalWrite(ledPins[index], LOW);   // turn LED off
}

void wavePulse(int stones[], float smooth, int waveType) {
  for (int i = 0; i < smooth; i++){
    float pwm_val = waveFunction(GAUSS, i, smooth);
    turnLEDS(stones, pwm_val);    
    delay(1);
  }
}

float waveFunction(int waveType, int k, float smooth) {
  float pwm = 0;
  switch(waveType) {
    
    case TRIANGLE:
      pwm = 255.0*(1.0 -  abs((2.0*(k/smooth))-1.0));
      break;

    case CIRCLE:
      pwm = 255.0*sqrt(1.0 -  pow(abs((2.0*(k/smooth))-1.0),2.0));
      break;

    default:
      pwm = 255.0*(exp(-(pow(((k/smooth)-beta)/gamma,2.0))/2.0));
      break;
  }

  return pwm;
}
