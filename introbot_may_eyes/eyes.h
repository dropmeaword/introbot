#ifndef __EYES_H__
#define __EYES_H__

#include <Arduino.h>
#include "corestats.h"

#include "config.h"
#include "RunningAverage.h"


class Eye {
  public:
    int LDR_PIN;

    // change the WEIGHing factors if you want to influence how much 
    // from each eye is taken into account
    float WEIGH = 1.0;

    AdaptiveNormalizer *mavg;
    float smoothedReading = 0;
    float SMOOTH_FACTOR = 0.8; // high values towards 1 = slow response, very smooth. 0 = input reading

//    RunningAverage *mavg; //( (1000/LDR_READ_EVERY_MS)*10 );
    
    float reading;

    float vmin, vmax;

    Eye(int iopin) {
      LDR_PIN = iopin;
      mavg = new AdaptiveNormalizer(0, 1, 0.0001);
//      mavg = new RunningAverage( (1000/LDR_READ_EVERY_MS) );
    }

//    ~Eye() {
//      delete mavg;
//    }
    
    float read() {
      reading = 0;

      // read 4 times to get a stable reading
      for (int i = 0; i < 4; i++) {
        reading += analogRead(LDR_PIN);
        //delay(5);
      }

      // update minmax
      vmax = max(reading, vmax);
      vmin = min(reading, vmin);

      reading = (reading / 4.0);
      smoothedReading = SMOOTH_FACTOR*smoothedReading + (1-SMOOTH_FACTOR)*reading;

      mavg->put(reading);
      if (mavg->get() == NAN) {
        mavg->reset();
      }
      
//      mavg->addValue(reading);
      
      return reading;
    }
    
    void debug() {
//      Serial.print( mavg->getAverage() );   // smooth value
      Serial.print( reading );   // smooth value
      Serial.print( "," );
      float val = mavg->get();
      Serial.print( smoothedReading );   // smooth value
//      Serial.print( "," );
//      Serial.print( mavg->var() );   // variability
    }
}; // class

class Eyes {
  public:

    Eye *left_;
    Eye *right_;

    Eyes(int pinLeft, int pinRight) {
//      Serial.print("left eye on pin: ");
//      Serial.print(pinLeft);
//      Serial.print("right eye on pin: ");
//      Serial.print(pinRight);
//      Serial.println();
//
      left_  = new Eye(pinLeft);
      right_ = new Eye(pinRight);
    }

    ~Eyes() {
      delete left_;
      delete right_;
    }

    Eye &left() { return (Eye &)left_; }
    Eye &right() { return (Eye &)right_; }
    
    float diff() { return ( (left_->smoothedReading * left_->WEIGH) - (right_->smoothedReading * right_->WEIGH) ); }

//    float diff() { return ( (left_->mavg->getAverage() * left_->WEIGH) - (right_->mavg->getAverage() * right_->WEIGH) ); }
    float absdiff() { return abs(diff()); } 
   
    float read() {
      left_->read();
      right_->read();
    }
    
    void debug() {
      left_->debug();
      Serial.print(",");
      right_->debug();
      Serial.print(",");
      float d = absdiff();
      Serial.print(d);
    }
}; // class

#endif // __EYES_H__
