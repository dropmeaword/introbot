#include <float.h>
#include "eyes.h"

AdaptiveNormalizer rnorm(0, 1, 0.0001);
AdaptiveNormalizer lnorm(0, 1, 0.0001);

Eye leye, reye;


float read_ldr(int ipin) {
  float reading = 0;

  // read 4 times to get a stable reading
  for (int i = 0; i < 4; i++) {
    reading += analogRead(ipin);
    delay(2);
  }

  reading = (reading / 4.0);
  return reading;
}

void eyes_init() {
  // L E F T   E Y E
  leye.iopin = LDR_LEFT;
  leye.weight = 1.0;
  leye.reading = 0;
  leye.vmax = FLT_MIN;
  leye.vmin = FLT_MAX;
  
  // RIGHT   E Y E
  reye.iopin = LDR_RIGHT;
  reye.weight = 1.0;
  reye.reading = 0;
  reye.vmax = FLT_MIN;
  reye.vmin = FLT_MAX;
}

void eyes_read() {
  // L E F T   E Y E
  // ///////////////
  // read eye light level from ldr sensor
  leye.reading = read_ldr( leye.iopin );

  // update minmax (used later for scaling)
  leye.vmax = max(leye.reading, leye.vmax);
  leye.vmin = min(leye.reading, leye.vmin);

  // smooth reading to remove noise
  leye.smoothedReading = SMOOTH_FACTOR*leye.smoothedReading + (1-SMOOTH_FACTOR)*leye.reading;

  lnorm.put(leye.reading);

  // RIGHT   E Y E
  // ///////////////
  reye.reading = read_ldr( reye.iopin );

  // update minmax (used later for scaling)
  reye.vmax = max(reye.reading, reye.vmax);
  reye.vmin = min(reye.reading, reye.vmin);

  // smooth reading to remove noise
  reye.smoothedReading = SMOOTH_FACTOR*reye.smoothedReading + (1-SMOOTH_FACTOR)*reye.reading;

  rnorm.put(reye.reading);
}

float eyes_diff() { 
  return ( (leye.smoothedReading * leye.weight) - (reye.smoothedReading * reye.weight) ); 
}

//    float eyes_diff() { return ( (left_->mavg->getAverage() * left_->WEIGH) - (right_->mavg->getAverage() * right_->WEIGH) ); }

float eyes_absdiff() { 
  return abs(eyes_diff()); 
} 

void debug_eye(Eye &e) {
  Serial.print( e.reading );
  Serial.print( "," );
  Serial.print( e.smoothedReading );
}

void eyes_debug() {
  debug_eye(leye);
  Serial.print( "," );
  debug_eye(reye);
  Serial.print( "," );
  Serial.print( lnorm.get() );
  Serial.print( "," );
  Serial.print( rnorm.get() );
}

