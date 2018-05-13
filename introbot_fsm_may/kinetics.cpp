#include "Arduino.h"

void Kinetics::init() {
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

  Serial.println( "initializing motors" );
}

void initDefaultSpeed(int defaultSpeed){
  DEFAULT_SPEED=defaultSpeed;
}
void initRightMotorPwm(int directionPin,int speedPin){
  MOTOR_RIGHT_SPEED=speedPin;
  MOTOR_RIGHT_DIRECTION=directionPin; 
  pinMode(MOTOR_RIGHT_DIRECTION, OUTPUT);     
}

void initLeftMotorPwm(int directionPin,int speedPin){
  MOTOR_LEFT_SPEED=speedPin;
  MOTOR_LEFT_DIRECTION=directionPin; 
  pinMode(MOTOR_LEFT_DIRECTION, OUTPUT);     
}


void Kinetics::go_forward() {
  digitalWrite(MOTOR_RIGHT_DIRECTION,HIGH);   
  digitalWrite(MOTOR_LEFT_DIRECTION, HIGH);  
}

void Kinetics::go_back() {
  digitalWrite(MOTOR_RIGHT_DIRECTION,LOW);   
  digitalWrite(MOTOR_LEFT_DIRECTION, LOW);       
}
 
void Kinetics::go(int speedValue){
  analogWrite(MOTOR_RIGHT_SPEED,speedValue);   
  analogWrite(MOTOR_LEFT_SPEED, speedValue);
}

void Kinetics::stop() {
  analogWrite(MOTOR_RIGHT_SPEED,0);   
  analogWrite(MOTOR_LEFT_SPEED, 0);   
}

void Kinetics::turn_left(int degrees) {
  analogWrite(MOTOR_RIGHT_SPEED,255);   
  digitalWrite(MOTOR_LEFT_DIRECTION, LOW);  
  analogWrite(MOTOR_LEFT_SPEED, 150);
  delay(degrees*4); 
  pwmSpeed(DEFAULT_SPEED);
  pwmForward();
}

void pwmTurnRight(int degrees){
  analogWrite(MOTOR_LEFT_SPEED,255);   
  digitalWrite(MOTOR_RIGHT_DIRECTION, LOW);  
  analogWrite(MOTOR_RIGHT_SPEED, 150);
  delay(degrees*4); 
  pwmSpeed(DEFAULT_SPEED);
  pwmForward();
}

void pwmTurnSmoothRight(int time){
  pwmForward();
  analogWrite(MOTOR_LEFT_SPEED,255);   
  analogWrite(MOTOR_RIGHT_SPEED, 150);
  delay(time*5); 
  pwmSpeed(DEFAULT_SPEED);
  pwmForward();
}
void pwmTurnSmoothLeft(int time){
  pwmForward();
  analogWrite(MOTOR_RIGHT_SPEED,255);   
  analogWrite(MOTOR_LEFT_SPEED, 150);
  delay(time*5); 
  pwmSpeed(DEFAULT_SPEED);
  pwmForward();
}

void showMotorsFeatures(){
  int value;
  if(demoIteration ==1 || demoIteration==3){
    pwmForward();
    pwmSpeed(200);  
  }
  if(demoIteration ==2 || demoIteration==4){
   pwmBackward();
    pwmSpeed(200);      
  }
  if(demoIteration==5){
      pwmTurnRight(30);
  }
  if(demoIteration==6){
      pwmTurnLeft(30);
  }
  
  if(demoIteration>6){
    pwmStop();
  }
  demoIteration+=1;
  delay(1000); 
  }
  
