#include "Arduino.h"
#include "kinetics.h"

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

  DEFAULT_SPEED = SPEED_MEDIUM;

  MOTOR_RIGHT_SPEED = CHA_PWM;
  MOTOR_RIGHT_DIRECTION = CHA_DIR; 

  MOTOR_LEFT_SPEED = CHB_PWM;
  MOTOR_LEFT_DIRECTION = CHB_DIR; 

  mmWheelRadius = WHEEL_RADIUS_IN_MM;
  mmWheelDistance = WHEEL_RADIUS_IN_MM;
  
  wheel_diameter_ = 2*mmWheelRadius;
  circumference_ = 2*PI * wheel_diameter_;
  max_rpm_ = MOTOR_MAX_RPM;
  base_width_ = mmWheelDistance;
  pwm_res_ = pow(2, 8) - 1;

  Serial.println( "initializing motors" );
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
  go(DEFAULT_SPEED);
  go_forward();
}

void Kinetics::turn_right(int degrees){
  analogWrite(MOTOR_LEFT_SPEED,255);   
  digitalWrite(MOTOR_RIGHT_DIRECTION, LOW);  
  analogWrite(MOTOR_RIGHT_SPEED, 150);
  delay(degrees*4); 
  go(DEFAULT_SPEED);
  go_forward();
}

void Kinetics::smooth_right(int time){
  go_forward();
  analogWrite(MOTOR_LEFT_SPEED,255);   
  analogWrite(MOTOR_RIGHT_SPEED, 150);
  delay(time*5); 
  go(DEFAULT_SPEED);
  go_forward();
}
void Kinetics::smooth_left(int time){
  go_forward();
  analogWrite(MOTOR_RIGHT_SPEED,255);   
  analogWrite(MOTOR_LEFT_SPEED, 150);
  delay(time*5); 
  go(DEFAULT_SPEED);
  go_forward();
}

void Kinetics::demo_loop() {
  boolean wheel1 = (millis() % 10000) < 5000;
  boolean forward = (millis() % 5000) < 2500;

  if (wheel1) {
    Serial.print("WHEEL A: ");
    analogWrite(CHA_PWM, 255);
    analogWrite(CHB_PWM, 0);
    Serial.println(forward ? "FORWARD" : "BACKWARD");
    digitalWrite(CHA_DIR, forward ? HIGH : LOW);
  } else {
    Serial.print("WHEEL B: ");
    analogWrite(CHA_PWM, 0);
    analogWrite(CHB_PWM, 255);
    Serial.println(forward ? "FORWARD" : "BACKWARD");
    digitalWrite(CHB_DIR, forward ? HIGH : LOW);
  }
}

void Kinetics::update() {
}


Kinetics::levels Kinetics::getRPM(float linear_x, float linear_y, float angular_z)
{
  //convert m/s to m/min
  linear_vel_x_mins_ = linear_x * 60;
  linear_vel_y_mins_ = linear_y * 60;

  //convert rad/s to rad/min
  angular_vel_z_mins_ = angular_z * 60;

  //Vt = ω * radius
  tangential_vel_ = angular_vel_z_mins_ * base_width_;

  x_rpm_ = linear_vel_x_mins_ / circumference_;
  y_rpm_ = linear_vel_y_mins_ / circumference_;
  tan_rpm_ = tangential_vel_ / circumference_;

  Kinetics::levels rpm;

  //calculate for the target motor RPM and direction
  
  //front-left motor
  rpm.motor1 = x_rpm_ - y_rpm_ - tan_rpm_;
  //front-right motor
  rpm.motor2 = x_rpm_ + y_rpm_ + tan_rpm_;

  return rpm;
}

int Kinetics::rpmToPWM(int rpm)
{
  //remap scale of target RPM vs MAX_RPM to PWM
  return (((double) rpm / (double) max_rpm_) * pwm_res_);
}

Kinetics::levels Kinetics::getPWM(float linear_x, float linear_y, float angular_z)
{
  Kinetics::levels rpm;
  Kinetics::levels pwm;

  rpm = getRPM(linear_x, linear_y, angular_z);

  //convert from RPM to PWM
  //front-left motor
  pwm.motor1 = rpmToPWM(rpm.motor1);
  //rear-left motor
  pwm.motor2 = rpmToPWM(rpm.motor2);

  return pwm;
}
