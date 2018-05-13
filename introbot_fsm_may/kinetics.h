#ifndef __KINETICS_H__
#define __KINETICS_H__

public class Kinetics {
  public:
    float speed;

    int MOTOR_RIGHT_SPEED;   
    int MOTOR_RIGHT_DIRECTION;
    int MOTOR_LEFT_SPEED;                         
    int MOTOR_LEFT_DIRECTION;                           
    int DEFAULT_SPEED;

    void init();
    void go_forward();
    void go_back();

    void stop();

    void go(int speed);

    void turn_left(int degrees);
    void turn_right(int degrees);
};

#endif
