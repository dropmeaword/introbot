#include "corestats.h"

#define LDR_Right A3
#define LDR_Left  A2

AdaptiveNormalizer mavgL(0,1);
AdaptiveNormalizer mavgR(0,1);

double eyer, eyel;

void setup() {
  Serial.begin(112500);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

double read_eye(int eye) {
  double reading = .0;
  // read 4 times to get a stable reading
  for (int i = 0; i < 4; i++) {
    reading += analogRead(eye);
    delay(5);
  }

  return (reading / 4.0);
}

void read_eyes() {

  eyer = read_eye(LDR_Right);
  mavgR.put( eyer );

  eyel = read_eye(LDR_Left);
  mavgL.put( eyel );

//  Serial.print(eyer);
//  Serial.print(", ");
//  Serial.print(eyel);
//  Serial.print(", ");
  Serial.print(mavgR.get());
  Serial.print(", ");
  Serial.print(mavgL.get());
  Serial.println();
}

void loop() {
  read_eyes();
  delay(50);
}
