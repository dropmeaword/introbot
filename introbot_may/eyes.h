#ifndef __EYES_H__
#define __EYES_H__

#include <Arduino.h>
#include "corestats.h"

class Eyes {
  public:
    int LEFT_LDR_PIN;
    int RIGHT_LDR_PIN;

    // change the WEIGHing factors if you want to influence how much 
    // from each eye is taken into account
    double WEIGH_L = 1.0;
    double WEIGH_R = 1.0;

    AdaptiveNormalizer *mavgl;
    AdaptiveNormalizer *mavgr;
    double right, left;

    Eyes(int pinLeft, int pinRight) {
      LEFT_LDR_PIN = pinLeft;
      RIGHT_LDR_PIN = pinRight;

      mavgl = new AdaptiveNormalizer(0, 1);
      mavgr = new AdaptiveNormalizer(0, 1);
    }

    ~Eyes() {
      delete mavgl;
      delete mavgr;
    }
    
    double diff();
    double read_eye(int pin);

    double read_left();    
    double read_right();
    
    double read_eyes();
    void debug();
}; // class

#endif // __EYES_H__
