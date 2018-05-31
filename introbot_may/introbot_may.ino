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


Shaker shaker(PIN_SHAKER);
Kinetics kinetics;

Flasher superfast(pinLEDS, LED_COUNT, 0.4);
Flasher slow(pinLEDS, LED_COUNT, 0.009);
Flasher fast(pinLEDS, LED_COUNT, 0.06);

// 0.009 nice and smooth
// 0.08 fast hearbeat
// 0.4 fast flashing

Chrono calmdown;
int stressCounter = 0;

Metro lookout(20); // sampling frequency of LDR sensing

// ///////////////////////////////////////////////////////////////////////////////////
// ON CALIBRATING
// ///////////////////////////////////////////////////////////////////////////////////
int threshold = 10.0;

void on_calibrating_enter() {
  Serial.println("on_calibrating_enter");
}

void on_calibrating_leave() {
  Serial.println("on_calibrating_leave");
  threshold = threshold + (threshold * 1.25);
  Serial.print("calculated threshold = ");
  Serial.println(threshold);
  eyes_reset_ranges();
  shaker.on(500);
}

void on_calibrating_loop() {
  threshold = ceil(eyes_abssum()); //max(leye.vmax, reye.vmax); //ceil(eyes_absdiff());
}

State calibrating("calibrating", &on_calibrating_enter, &on_calibrating_loop, &on_calibrating_leave);

// ///////////////////////////////////////////////////////////////////////////////////
// ON DEMO
// ///////////////////////////////////////////////////////////////////////////////////
void on_demo_enter() {
  Serial.println("on_demo_enter");
  //shaker.on(5000);
}

void on_demo_leave() {
  Serial.println("on_demo_leave");
  kinetics.stop();
  shaker.off();
}

void on_demo_loop() {
  kinetics.demo_loop();
  shaker.update();
}

State demo("demo", &on_demo_enter, &on_demo_loop, &on_demo_leave);

//Fsm fsm(&calibrating);
Fsm fsm(&demo);

// ///////////////////////////////////////////////////////////////////////////////////
// ON HAPPY
// ///////////////////////////////////////////////////////////////////////////////////
void on_happy_enter() {
  Serial.println("on_happy_enter");
  slow.flash();
}

void on_happy_leave() {
  Serial.println("on_happy_leave");
  slow.stop();
}

void on_happy_loop() {
  slow.update();

  if(eyes_abssum() > threshold) {
      fsm.trigger(EVENT_GOT_STRESSED);
  }
}

State happy("happy", &on_happy_enter, &on_happy_loop, &on_happy_leave);

// ///////////////////////////////////////////////////////////////////////////////////
// ON STRESSED
// ///////////////////////////////////////////////////////////////////////////////////
void on_stressed_enter() {
  Serial.println("on_stressed_enter");
  fast.flash();
  shaker.on(500);
  stressCounter++;
}

void on_stressed_leave() {
  Serial.println("on_stressed_leave");
  shaker.off();
  fast.stop();
}

void on_stressed_loop() {
//  Serial.println("on_stressed_loop");
  fast.update();
  shaker.update();

  kinetics.steer_with_light();

  if(stressCounter > 3) {
    fsm.trigger(EVENT_GOT_PARANOID);
  }

  // the following 2 if statements basically tell the bot how to calm down
  if(eyes_abssum() > threshold) {
    calmdown.restart();
  }

  if(calmdown.hasPassed(2000)) {
    fsm.trigger(EVENT_GOT_HAPPY);
  }
}


State stressed("stressed", &on_stressed_enter, &on_stressed_loop, &on_stressed_leave);

// ///////////////////////////////////////////////////////////////////////////////////
// ON PARANOID
// ///////////////////////////////////////////////////////////////////////////////////
Metro cycle(500);

void on_paranoid_enter() {
  Serial.println("on_paranoid_enter");
  fast.flash();
  shaker.on(500);
  calmdown.restart();
}

void on_paranoid_leave() {
  Serial.println("on_paranoid_leave");
  fast.stop();
  shaker.off();
  stressCounter = 0;
}

void on_paranoid_loop() {
  fast.update();

  shaker.on(500);
  shaker.update();

  // spin randomly
  if(cycle.check()) {
    int chooser = random(3);
    switch(chooser) {
      case 0:
        kinetics.turn_right();
        break;
      case 1:
        kinetics.turn_left();
        break;
      case 2:
        kinetics.stop();
        break;
    } // switch
    //cycle.interval(random(80,500));
  } // if

  // continue in this state or calmdown?
  if(eyes_abssum() > threshold) {
    calmdown.restart();
  }

  if(calmdown.hasPassed(10000)) {
    fsm.trigger(EVENT_GOT_HAPPY);
  }
}

State paranoid("paranoid", &on_paranoid_enter, &on_paranoid_loop, &on_paranoid_leave);

// ///////////////////////////////////////////////////////////////////////////////////
// TRANSITIONS
// ///////////////////////////////////////////////////////////////////////////////////
void on_trans_happy_to_stressed()
{
  Serial.println("Transitioning from HAPPY to STRESSED");
}

void on_trans_stressed_to_paranoid()
{
  Serial.println("Transitioning from STRESSED to PARANOID");
}

void on_trans_stressed_to_happy()
{
  Serial.println("Transitioning from STRESSED to HAPPY");
}


// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  //while(!Serial);

  Serial.println("setup()");

  eyes_init();

  kinetics.init();
  kinetics.stop();

  Serial.println("setup");
 
  // state transition configuration
  fsm.add_timed_transition(&demo, &calibrating, 10000, NULL);
  fsm.add_timed_transition(&calibrating, &happy, 5000, NULL);
//  fsm.add_timed_transition(&demo, &happy, 25000, NULL);
  fsm.add_transition(&happy, &stressed, EVENT_GOT_STRESSED, &on_trans_happy_to_stressed);
  fsm.add_transition(&stressed, &happy, EVENT_GOT_HAPPY, &on_trans_stressed_to_happy);
  fsm.add_transition(&stressed, &paranoid, EVENT_GOT_PARANOID, &on_trans_stressed_to_paranoid);
  fsm.add_transition(&paranoid, &happy, EVENT_GOT_HAPPY, NULL);
}


void bot_loop() {
  if( lookout.check() ) {
    eyes_read();
//    eyes_debug();
//    Serial.print(", ");
//    Serial.println(threshold);
  }

  shaker.update();
  kinetics.update();
}

void loop() {
  bot_loop();
  fsm.run_machine();
}

