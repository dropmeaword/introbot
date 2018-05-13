#include "eyes.h"

double Eyes::diff() {
  return ((left * WEIGH_L) - (right * WEIGH_R));
}

double Eyes::read_eye(int iopin) {
  double reading = .0;
  
  // read 4 times to get a stable reading
  for (int i = 0; i < 4; i++) {
    reading += analogRead(iopin);
    delay(5);
  }

  return (reading / 4.0);
}

double Eyes::read_left() {   
  left  = read_eye(LEFT_LDR_PIN);
  mavgl->put( left );
  return left;
}

double Eyes::read_right() { 
  right = read_eye(RIGHT_LDR_PIN);
  mavgr->put( right );
  return right;
}

double Eyes::read_eyes() {
  read_left();
  read_right();
  return diff();
}

void Eyes::debug() {
  Serial.print( mavgl->get() );   // smooth value
  Serial.print( "," );
  Serial.print( mavgr->get() );
  Serial.print( "," );
  Serial.print( mavgl->var() );   // variability
  Serial.print( "," );
  Serial.print( mavgr->var() );
  Serial.print( "," );
}

