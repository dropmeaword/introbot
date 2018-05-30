#include <SoftwareSerial.h>

#include "statemachine.h"
#include "flasher.h"
#include "config.h"

#include "kinetics.h"
#include "eyes.h"
#include "shaker.h"

#define LED_COUNT 3
int pinLEDS[LED_COUNT] = {10, 5, 6};

#define EVENT_GOT_HAPPY    1000
#define EVENT_GOT_STRESSED 1100
#define EVENT_GOT_PARANOID 1200


Eyes eyes(LDR_LEFT, LDR_RIGHT);
Shaker shaker(PIN_SHAKER);
Kinetics kinetics;

int threshold = 8;

Flasher superfast(pinLEDS, LED_COUNT, 0.4);
Flasher slow(pinLEDS, LED_COUNT, 0.009);
Flasher fast(pinLEDS, LED_COUNT, 0.06);

// 0.009 nice and smooth
// 0.08 fast hearbeat
// 0.4 fast flashing

Chrono calmdown;
Chrono score;
int stressCounter = 0;

bool forth = true;
Metro metro(500);

// ///////////////////////////////////////////////////////////////////////////////////
// ON DEMO
// ///////////////////////////////////////////////////////////////////////////////////
void on_demo_enter() {
  Serial.println("on_demo_enter");
  score.restart();
}

void on_demo_leave() {
  Serial.println("on_demo_leave");
  kinetics.stop();
}

void on_demo_loop() {
  bool skip = false;
  
  if( metro.check() ) {
    forth = !forth;    
  }

  if(score.hasPassed(15000)) {
    skip = true;
    if(forth) {
      kinetics.turn_left();
    } else {
      kinetics.turn_right();
    }
  }

  if(!skip && score.hasPassed(10)) {
    if(forth) {
      kinetics.go_forward();
      kinetics.go(255);
    } else {
      kinetics.go_back();
      kinetics.go(255);
    }
  }
}

State demo("demo", &on_demo_enter, &on_demo_loop, &on_demo_leave);
Fsm fsm(&demo);

// ///////////////////////////////////////////////////////////////////////////////////
// ON HAPPY
// ///////////////////////////////////////////////////////////////////////////////////
void on_happy_enter() {
  Serial.println("on_happy_enter");
}

void on_happy_leave() {
  Serial.println("on_happy_leave");
}

void on_happy_loop() {
}

State happy("happy", &on_happy_enter, &on_happy_loop, &on_happy_leave);


// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("setup()");
 
  kinetics.init();
  kinetics.stop();

  // state transition configuration
  fsm.add_timed_transition(&demo, &happy, 30000, NULL);

  Serial.println("fsm up");
}


void bot_loop() {
//  eyes.read();
//  eyes.debug();
//  Serial.print(",");

  kinetics.update();

//  Serial.print(", ");
//  Serial.println(threshold);
}

void loop() {
  bot_loop();
  fsm.run_machine();
}

