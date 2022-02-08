
#include "avdweb_VirtualDelay.h"
#include <ButtonDebounce.h>
#include <Streaming.h>

#define SPACE 0
#define MIND 1
#define SOUL 2
#define REALITY 3
#define TIME 4
#define POWER 5

#define NUM_OF_LEDS 6
int ledPins[] = {3, 5, 6, 9, 10, 11};
int stones[] = {0, 1, 2, 3, 4, 5};

ButtonDebounce button14(14, 250);
ButtonDebounce button15(15, 250);
ButtonDebounce button16(16, 250);
ButtonDebounce button17(17, 250);
ButtonDebounce button18(18, 250);


float inhale = 0, inhale_retention = 0, exhale = 0, exhale_retention = 0;
VirtualDelay delayInhale, delayInhaleHold, delayExhale, delayExhaleHold, delayBlink;

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
    sinePulseNonBlocking(inhale, inhale_retention, exhale, exhale_retention);
  }
}

void sinePulseNonBlocking(float inhale_speed, float inhale_retention, float exhale_speed, float exhale_retention) {
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
      in = 4.10;
    }
  }

  // top
  if (in > 7.85 && in < 7.86) {
    delayInhaleHold.start(inhale_retention);
    in = delayInhaleHold.elapsed() ? 7.86 : 6.8;
  }
  
  out = sin(in) * 127.5 + 127.5;
  turnLEDS(out);
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
    randomizeStones();
    inhale = 1.5;
    inhale_retention = 3500;
    exhale = 1.5;
    exhale_retention = 3500;
  }
  else if (button15.state() == LOW) {
    randomEffect();
    // 1-1-1-1  Square Breathing, preserve energy
    randomizeStones();
    inhale = 3;
    inhale_retention = 1000;
    exhale = 3;
    exhale_retention = 1000;
  }
  else if (button16.state() == LOW) {
    randomEffect();
    // 1-2  Paced Breathing, stress reduction
    randomizeStones();
    inhale = 3;
    inhale_retention = 1000;
    exhale = 1.5;
    exhale_retention = 500;
  }
  else if (button17.state() == LOW) {
    randomEffect();
    // 4-7-8 Sleeping. extreme relaxation
    randomizeStones();
    inhale = 2.5;
    inhale_retention = 6500;
    exhale = 1;
    exhale_retention = 7500;
  }
  else if (button18.state() == LOW) {
    randomEffect();
    // 1-2-1 Awake. Energizing
    randomizeStones();
    inhale = 6;
    inhale_retention = 200;
    exhale = 6;
    exhale_retention = 100;
  }
  
}

void turnLEDS(int out) {
  for (int i = 0; i < NUM_OF_LEDS; i++) {
    analogWrite(ledPins[stones[i]], out);
  }
}

void turnLEDSOff() {
  for (int i = 0; i < NUM_OF_LEDS; i++) {
    analogWrite(ledPins[i], 0);
  }
  
  inhale = 0;
  inhale_retention = 0;
  exhale = 0;
  exhale_retention = 0;
}

void randomizeStones() {
  stones[0] = random(0, 6);
  stones[1] = random(0, 6);
  stones[2] = random(0, 6);
  stones[3] = random(0, 6);
  stones[4] = random(0, 6);
  stones[5] = random(0, 6);
}

void randomEffect() {
  turnLEDSOff();
  
  int k = random(0, 5);

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
