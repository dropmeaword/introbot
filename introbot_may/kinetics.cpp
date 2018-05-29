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
  
  ml.dir = FORTH;
  ml.speed = 0;
  mr.dir = FORTH;
  mr.speed = 0;

  Serial.println( "initializing motors" );
}

void Kinetics::go_forward() {
  ml.dir = FORTH;
  mr.dir = BACK;
}

void Kinetics::go_back() {
  ml.dir = BACK;
  mr.dir = FORTH;
}
 
void Kinetics::go(int val){
  ml.speed = val;
  mr.speed = val;
}

void Kinetics::stop() {
  Serial.println("kinetics::stop");
  ml.dir = 0;
  ml.speed = 0;
  mr.dir = 0;
  mr.speed = 0;
}

void Kinetics::turn_left(int degrees) {
  ml.dir = BACK;
  ml.speed = 255;
  mr.dir = FORTH;
  mr.speed = 255;
  
  lastaction.interval(degrees * 4);
  lastaction.reset();

//  analogWrite(MOTOR_RIGHT_SPEED,255);   
//  digitalWrite(MOTOR_LEFT_DIRECTION, LOW);  
//  analogWrite(MOTOR_LEFT_SPEED, 150);
//  delay(degrees*4); 
//  go(DEFAULT_SPEED);
//  go_forward();
}

void Kinetics::turn_right(int degrees){
  ml.dir = FORTH;
  ml.speed = 255;
  mr.dir = BACK;
  mr.speed = 255;

  lastaction.interval(degrees * 4);
  lastaction.reset();

//  analogWrite(MOTOR_LEFT_SPEED,255);   
//  digitalWrite(MOTOR_RIGHT_DIRECTION, LOW);  
//  analogWrite(MOTOR_RIGHT_SPEED, 150);
//  delay(degrees*4); 
//  go(DEFAULT_SPEED);
//  go_forward();
}

void Kinetics::smooth_right(int time){
  ml.dir = FORTH;
  ml.speed = 255;
  mr.dir = FORTH;
  mr.speed = 180;
  
  lastaction.interval(time * 5);
  lastaction.reset();  
//  go_forward();
//  analogWrite(MOTOR_LEFT_SPEED,255);   
//  analogWrite(MOTOR_RIGHT_SPEED, 150);
//  delay(time*5); 
//  go(DEFAULT_SPEED);
//  go_forward();
}
void Kinetics::smooth_left(int time){
  ml.dir = FORTH;
  ml.speed = 180;
  mr.dir = FORTH;
  mr.speed = 255;
  
  lastaction.interval(time * 5);
  lastaction.reset();  
//  go_forward();
//  analogWrite(MOTOR_RIGHT_SPEED,255);   
//  analogWrite(MOTOR_LEFT_SPEED, 150);
//  delay(time*5); 
//  go(DEFAULT_SPEED);
//  go_forward();
}

void Kinetics::demo_loop() {

  if(sem_turn_left && demotimer.hasPassed(100) && !demotimer.hasPassed(5000) ) {
    Serial.println("forward");
    go_forward();
    go(255);
    sem_turn_left = false;
  } 
  
  if(sem_turn_right && demotimer.hasPassed(5000) && !demotimer.hasPassed(10000) ) {
    Serial.println("back");
    go_back();
    go(255);
    sem_turn_right = false;
  }
  
  if(sem_smooth_right && demotimer.hasPassed(10000) && !demotimer.hasPassed(15000) ) {
    Serial.println("smooth right");
    smooth_right(560);
    sem_smooth_right = false;
  } 
  
  if(sem_smooth_left && demotimer.hasPassed(15000) && !demotimer.hasPassed(20000) ) {
    Serial.println("smooth left");
    smooth_left(560);
    sem_smooth_left = false;
  }
  
  if(demotimer.hasPassed(20000)) {
    demotimer.restart();
    sem_smooth_right = true;
    sem_smooth_left = true;
    sem_turn_left = true;
    sem_turn_right = true;
  }

//  update();
}

void Kinetics::update() {
//  if(mr.dir == 0) {
//    stop();
//  }
//

//  Serial.print(" r.dir: ");
//  Serial.print(MOTOR_RIGHT_DIRECTION);
//  Serial.print(" r.speed: ");
//  Serial.print(MOTOR_RIGHT_SPEED);
//  Serial.print(" l.dir: ");
//  Serial.print(MOTOR_LEFT_DIRECTION);
//  Serial.print(" l.speed: ");
//  Serial.print(MOTOR_LEFT_SPEED);
//  Serial.println();
  
  digitalWrite(MOTOR_RIGHT_DIRECTION, mr.dir);   
  digitalWrite(MOTOR_LEFT_DIRECTION, ml.dir);  

  analogWrite(MOTOR_RIGHT_SPEED, mr.speed);   
  analogWrite(MOTOR_LEFT_SPEED, ml.speed);

  // if lastaction is completed, stop
  if(lastaction.check() && !stopped()) {
    stop();
  }
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

int Kinetics::mappedPwmValue(int reading, int vmin, int vmax) {
  int val;

  val = map(reading, vmin, vmax, 0, 1023);
  val = val / 4;

  return constrain(val, 0, 255);
}

void Kinetics::steer_with_light(Eyes &eyes) {
  ml.speed = mappedPwmValue(eyes.left().reading, eyes.left().vmin, eyes.left().vmax);
  mr.speed = mappedPwmValue(eyes.right().reading, eyes.right().vmin, eyes.right().vmax);
}























/*
Kinetics::velocities Kinetics::getVelocities(int motor1, int motor2)
{
  Kinetics::velocities vel;

  double average_rpm_x = (motor1 + motor2) / 2; // RPM
  //convert revolutions per minute to revolutions per second
  double average_rps_x = average_rpm_x / 60; // RPS
  vel.linear_x = (average_rps_x * (wheel_diameter_ * PI)); // m/s

  double average_rpm_a = (motor2 - motor1) / 2;
  //convert revolutions per minute to revolutions per second
  double average_rps_a = average_rpm_a / 60;
  vel.angular_z =  (average_rps_a * (wheel_diameter_ * PI)) / base_width_;

  return vel;
}

Kinetics::velocities Kinetics::getVelocities(int motor1, int motor2, int motor3, int motor4)
{
  Kinetics::velocities vel;

  double average_rpm_x = (motor1 + motor2 + motor3 + motor4) / 4; // RPM
  //convert revolutions per minute to revolutions per second
  double average_rps_x = average_rpm_x / 60; // RPS
  vel.linear_x = (average_rps_x * (wheel_diameter_ * PI)); // m/s

  double average_rpm_y = (-motor1 + motor2 + motor3 - motor4) / 4; // RPM
  //convert revolutions per minute in y axis to revolutions per second
  double average_rps_y = average_rpm_y / 60; // RPS
  vel.linear_y = (average_rps_y * (wheel_diameter_ * PI)); // m/s

  double average_rpm_a = (-motor1 + motor2 - motor3 + motor4) / 4;
  //convert revolutions per minute to revolutions per second
  double average_rps_a = average_rpm_a / 60;
  vel.angular_z =  (average_rps_a * (wheel_diameter_ * PI)) / base_width_;

  return vel;
}
 */
