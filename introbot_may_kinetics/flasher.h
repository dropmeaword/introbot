#ifndef __FLASHER_H__
#define __FLASHER_H__

#include <Arduino.h>

class Flasher
{
  int ledPin;      // the number of the LED pin

  int *iopins;
  int ledCount;
  
  long OnTime;     // milliseconds of on-time
  long OffTime;    // milliseconds of off-time

  // These maintain the current state
  int ledState;                 // ledState used to set the LED
  unsigned long previousMillis;   // will store last time LED was updated

  bool flashing;
  unsigned long flashingStarted;
  unsigned long flashingTime;

  float level, angle, av, amplitude;
  
  public:

  Flasher(int *pins, int count, float freq) {
    iopins = new int[count];

    // copy pin numbers
    for(int i = 0; i < count; i++) {
      iopins[i] = pins[i];
    }
    
    init(freq);
  }

  Flasher(int pin, float freq)
  {
    iopins = new int[1];
    iopins[0] = pin;
    ledCount = 1;

    init(freq);
  }

  ~Flasher() {
    delete []iopins;
  }


  void init(float freq) {
    for(int i = 0; i < ledCount; i++) {
      pinMode(iopins[i], OUTPUT);
    }
      
    ledState = LOW; 
    previousMillis = 0;

    angle = 0;
    av = freq;
    amplitude = 128;
  }

  void flash(int howlong = 1000) {
    flashing = true;
    flashingStarted = millis();
    flashingTime = howlong;
  }

  void stop() {
    flashing = false;
    for(int i = 0; i < ledCount; i++) {
      digitalWrite(iopins[i], LOW);
    }
  }

  void debug(){
    Serial.print(level);
    Serial.print(", ");
  }

  double xmap(double x, double in_min, double in_max, double out_min, double out_max)
  {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  double smooth() {
    angle += av;
    return 128+(amplitude * cos(angle));
//    return (exp(sin(millis()/(float(OnTime+OffTime))*PI)) - 0.36787944) * 108.0;
//    double val = 255*exp(cos(angle))*0.01;
//    return xmap(val, 0.9, 7.0, .0, 255.0);
  }

  float onoff() {
      float val = .0;
      unsigned long currentMillis = millis();

      if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
      {
        ledState = LOW;  // Turn it off
        previousMillis = currentMillis;  // Remember the time
        val = .0;
      }
      else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
      {
        ledState = HIGH;  // turn it on
        previousMillis = currentMillis;   // Remember the time
        val = 255.0;
      }
      return val;
  }
  
  void update()
  {
    // check to see if it's time to change the state of the LED
    level = smooth();

    if(flashing) {
      for(int i = 0; i < ledCount; i++) {
        analogWrite(iopins[i], level);
      }
    }
  } // update ()

}; // class

#endif // __FLASHER_H__
