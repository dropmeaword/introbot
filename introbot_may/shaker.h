#ifndef __SHAKER_H__
#define __SHAKER_H__

class Shaker {
    int iopin;
    int state;
    Metro timer;

  public:
    Shaker(int p) : iopin(p) {
      pinMode(iopin, OUTPUT);
    }

    void on(int howlong = 3000) {
      timer.interval(howlong);
      timer.reset();
      state = HIGH;
      update();
    }
    
    void off() {
      state = LOW;
      update();
    }

    void update() {
      if(timer.check()) {
        state = LOW;
      }
      digitalWrite(iopin, state);
    }
}; // class

#endif // __SHAKER_H__
