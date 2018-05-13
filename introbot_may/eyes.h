#ifndef __EYES_H__
#define __EYES_H__

#include <Arduino.h>
#include "corestats.h"

class Eye {
  public:
    int LDR_PIN;

    // change the WEIGHing factors if you want to influence how much 
    // from each eye is taken into account
    double WEIGH = 1.0;

    AdaptiveNormalizer *mavg;
    double reading;

    int vmin, vmax;

    Eye(int iopin) {
      LDR_PIN = iopin;
      mavg = new AdaptiveNormalizer(0, 1);
    }

    ~Eye() {
      delete mavg;
    }
    
    double read() {
      double reading = .0;
      
      // read 4 times to get a stable reading
      for (int i = 0; i < 4; i++) {
        reading += analogRead(LDR_PIN);
        //delay(5);
      }

      // update minmax
      vmax = max(reading, vmax);
      vmin = min(reading, vmin);
    
      return (reading / 4.0);
    }
    
    void debug() {
      Serial.print( mavg->get() );   // smooth value
      Serial.print( "," );
      Serial.print( mavg->var() );   // variability
    }
}; // class

class Eyes {
  public:

    Eye *left_;
    Eye *right_;

    Eyes(int pinLeft, int pinRight) {
      left_  = new Eye(pinLeft);
      right_ = new Eye(pinRight);
    }

    ~Eyes() {
      delete left_;
      delete right_;
    }

    Eye &left() { return (Eye &)left_; }
    Eye &right() { return (Eye &)right_; }
    
    double diff() { return ( (left_->reading * left_->WEIGH) - (right_->reading * right_->WEIGH) ); }
    
    double read() {
      left_->read();
      right_->read();
    }
    
    void debug() {
      left_->debug();
      Serial.print(",");
      right_->debug();
    }
}; // class

#endif // __EYES_H__
