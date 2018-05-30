#ifndef __KINETICS_H__
#define __KINETICS_H__

#include <Chrono.h>
#include <Metro.h>

#include "eyes.h"
#include "config.h"

// measurements of or 2WD bot
// these are used to calculate relationships between 
// revolutions and linear distances
#define DISTANCE_BETWEEN_WHEELS_IN_MM 100
#define WHEEL_RADIUS_IN_MM 30
#define LINEAR_DISTANCE_MM_PER_TURN 2*PI*30

#define MOTOR_MAX_RPM 60

// default speed settings for robot
#define SPEED_SLOW 50
#define SPEED_MEDIUM 100
#define SPEED_FAST 150

#define BACK LOW
#define FORTH HIGH

class Kinetics {
  public:

    struct motor {
      int dir;
      int speed;
    };
    
    struct levels {
      int motor1;
      int motor2;
    };

    struct velocity {
      float linear_x;
      float linear_y;
      float angular_z;
    };

    motor ml, mr; // motors left and right

    bool sem_turn_left = true, sem_turn_right = true, sem_smooth_right = true, sem_smooth_left = true;
  

    float speed;

    float mmWheelRadius;
    float mmWheelDistance;

    int MOTOR_RIGHT_SPEED;   
    int MOTOR_RIGHT_BREAK;   
    int MOTOR_RIGHT_DIRECTION;
    int MOTOR_LEFT_SPEED;                         
    int MOTOR_LEFT_BREAK;   
    int MOTOR_LEFT_DIRECTION;                           
    int DEFAULT_SPEED;

    void init();
    void go_forward();
    void go_back();

    void stop();
    void update();
    void debug();

    void go(int speed);

    void turn_left(int degrees = 100);
    void turn_right(int degrees = 100);

    void smooth_right(int time);
    void smooth_left(int time);
    void demo_loop();

    void steer_with_light(Eyes &eyes);

//    velocities getVelocities(int motor1, int motor2);
//    velocities getVelocities(int motor1, int motor2, int motor3, int motor4);
    levels getRPM(float linear_x, float linear_y, float angular_z);
    levels getPWM(float linear_x, float linear_y, float angular_z);
    int rpmToPWM(int rpm);

    bool stopped() { return ((ml.speed == 0) && (mr.speed == 0)); }
    bool isMoving() { return ((ml.speed > 0) && (mr.speed > 0)); }

    int mappedPwmValue(int reading, int vmin, int vmax);

private:

    Chrono demotimer;
    Metro lastaction;

    float linear_vel_x_mins_;
    float linear_vel_y_mins_;
    float angular_vel_z_mins_;
    float circumference_;
    float tangential_vel_;
    float x_rpm_;
    float y_rpm_;
    float tan_rpm_;
    int max_rpm_;
    double wheel_diameter_;
    float base_width_;
    double pwm_res_;
};

#endif
