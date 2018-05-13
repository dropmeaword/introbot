#include "flasher.h"

const int leds[3] = {10, 6, 5};

Flasher led1(leds[0], 800, 2000);
Flasher led2(leds[1], 800, 2000);
Flasher led3(leds[2], 800, 2000);

int threshold = 160;

void setup() {
  Serial.begin(112500);
  pinMode(A0, INPUT);
//  led.flash();
}

void loop() {
  // read analog sensor for interaction
  int reading = 0;
  for(int i = 0; i < 4; i++) {
    reading += analogRead(A0);
  }
  reading == reading >> 2;

  // flash if threshold is overflown, stop otherwise
  if(reading > threshold) {
    led1.flash();
    led2.flash();
    led3.flash();
  } else {
    led1.stop();
    led2.stop();
    led3.stop();
  }

  led1.update(); // update the flasher
  led2.update(); // update the flasher
  led3.update(); // update the flasher
//  led.debug();
  
  Serial.print(threshold);
  Serial.print(", ");
  Serial.println(reading);
  
  delay(10);
}
