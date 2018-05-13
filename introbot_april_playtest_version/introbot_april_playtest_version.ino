#include "corestats.h"

#define WILL
#include "personality.h"

#include "config.h"

int motorB_speed = 0;

int BEHAVIOUR_NORMAL = 0;
int BEHAVIOUR_PARANOID = 1;
int behaviourMode = BEHAVIOUR_NORMAL;

boolean lightWasShined = false;
int paranoidCounter = 0;               // Count the light flashes

AdaptiveNormalizer mavgL(0,1);
AdaptiveNormalizer mavgR(0,1);

int leftEye, rightEye;

void init_motor() {
  // initialize the PWM pins
  pinMode(CHB_PWM, OUTPUT);
  digitalWrite(CHB_PWM, LOW);
  // initialize the PWM pins
  pinMode(CHA_PWM, OUTPUT);
  digitalWrite(CHA_PWM, LOW);

  pinMode(CHA_DIR, OUTPUT);
  pinMode(CHA_BRK, OUTPUT);
  digitalWrite(CHA_DIR, LOW);
  digitalWrite(CHA_BRK, LOW);
  pinMode(CHB_DIR, OUTPUT);
  pinMode(CHB_BRK, OUTPUT);
  digitalWrite(CHB_DIR, LOW);
  digitalWrite(CHB_BRK, LOW);

  Serial.println( "initializing motor" );                                                                                                               
}

void setup() {
  Serial.begin(115200);

  init_motor();

  pinMode(VIBRATION_PIN, OUTPUT);
  pinMode (LED_PIN, OUTPUT);
}

void debug_text() {
  Serial.print("left sensor: ");
  Serial.println(leftEye);
  Serial.print("right sensor: ");
  Serial.println(rightEye);
}

void debug_graph() {
  Serial.print( mavgL.get() );
  Serial.print(",");
  Serial.println( mavgR.get() );
}

void read_eyes() {
  rightEye = readEye(LDR_Right);
  mavgR.put( rightEye );

  leftEye = readEye(LDR_Left);
  mavgL.put( leftEye );
}

void blinking_light () {
  digitalWrite (LED_PIN,HIGH);
  delay (2000);
  digitalWrite (LED_PIN,LOW);
  delay (2000);
}
  

void steer_with_light() {
  // put your main code here, to run repeatedly:
  analogWrite(CHB_PWM, mappedMotorValue(leftEye, true)); //conect sensor value to direction
  analogWrite(CHA_PWM, mappedMotorValue(rightEye, false));
}

void loop() {
  debug_graph();

  read_eyes();

  // normal behaviour:
  if (behaviourMode == BEHAVIOUR_NORMAL) {
    steer_with_light();
    if (mavgR.get() > (0.4*SENSITIVITY) || mavgL.get() > (0.4*SENSITIVITY) ) { // when the room is really dark it should be around 400. Maximun is 1000.
      shake(true);
    }
    else if (mavgR.get() < (0.4*SENSITIVITY) || mavgL.get() < (0.4*SENSITIVITY)) { // if there is no light stops vibrating
      shake(false);
      blinking_light();
    }

    // check for paranoid light:
    if (mavgR.get() > (0.6*SENSITIVITY) || mavgL.get() > (0.6*SENSITIVITY)) { 
      
      if (lightWasShined==false) {
        
        paranoidCounter++;
        Serial.print("HELP ");
        Serial.println(paranoidCounter);
        
        if (paranoidCounter==3) {
          behaviourMode = BEHAVIOUR_PARANOID;
          paranoidCounter = 0;
        }
      }
      lightWasShined = true;
    } else {
      lightWasShined = false;
    }
    
  }

  // PARANOID
  else if (behaviourMode == BEHAVIOUR_PARANOID) { 
    shake(true);
    if (random(2)<1) {
      analogWrite(CHB_PWM, 255); //conect sensor value to direction
      analogWrite(CHA_PWM, 0);
    } else {
      analogWrite(CHB_PWM, 0); //conect sensor value to direction
      analogWrite(CHA_PWM, 255);
    }
    delay(2000);
    behaviourMode = BEHAVIOUR_NORMAL;
  }
  
  delay(50);

}
double readEye(int eye) {
  double reading = .0;
  // read 4 times to get a stable reading
  for (int i = 0; i < 4; i++) {
    reading += analogRead(eye);
    delay(5);
  }

  return (reading / 4.0);
}


void shake(bool _shakeOn) {
  digitalWrite (VIBRATION_PIN, _shakeOn); // if the light is turned on, it activates the vibrating module
  //  delay (1000);
  //  digitalWrite (5, LOW);
  //  delay (1000);
}

int mappedMotorValue(int reading, bool isLeft) {
  int mappedValue;
  if (isLeft) {
    mappedValue = map(reading, LGT_THR_LFT_MIN, LGT_THR_LFT_MAX, 0, 1023);
  } else {
    mappedValue = map(reading, LGT_THR_RGT_MIN, LGT_THR_RGT_MAX, 0, 1023);
  }
  mappedValue = mappedValue / 4;
  if (mappedValue < 0) return 0;
  // --------------------------
  if (mappedValue > 255) return 255;
  return mappedValue;
}

