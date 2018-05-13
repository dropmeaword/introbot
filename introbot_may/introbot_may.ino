#include "statemachine.h"
#include "flasher.h"
#include "config.h"

#include "kinetics.h"
#include "eyes.h"
#include "shaker.h"

int pinLEDS[3] = {10, 5, 6};

#define EVENT_GOT_HAPPY    1000
#define EVENT_GOT_STRESSED 1100
#define EVENT_GOT_PARANOID 1200


Eyes eyes(LDR_LEFT, LDR_RIGHT);
Shaker shaker(PIN_SHAKER);
Kinetics kinetics;

int threshold = 8;

Flasher superfast(pinLEDS, 3, 0.4);
Flasher slow(pinLEDS, 3, 0.009);
Flasher fast(pinLEDS, 3, 0.06);

// 0.009 nice and smooth
// 0.08 fast hearbeat
// 0.4 fast flashing

Chrono calmdown;
int stressCounter = 0;

// ///////////////////////////////////////////////////////////////////////////////////
// ON CALIBRATING
// ///////////////////////////////////////////////////////////////////////////////////
void on_calibrating_enter() {
  Serial.println("on_calibrating_enter");
  superfast.flash();
}

void on_calibrating_leave() {
  Serial.println("on_calibrating_leave");
  superfast.stop();
}

void on_calibrating_loop() {
  superfast.update();
}

State calibrating("calibrating", &on_calibrating_enter, &on_calibrating_loop, &on_calibrating_leave);
Fsm fsm(&calibrating);

// ///////////////////////////////////////////////////////////////////////////////////
// ON DEMO
// ///////////////////////////////////////////////////////////////////////////////////
void on_demo_enter() {
  Serial.println("on_demo_enter");
  shaker.on(5000);
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
  if(eyes.absdiff() > threshold) {
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
  shaker.on(5000);
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

  kinetics.steer_with_light( eyes );
  kinetics.update();

  if(stressCounter > 3) {
    fsm.trigger(EVENT_GOT_PARANOID);
  }

  if(eyes.absdiff() > threshold) {
    calmdown.restart();
  }

  if(calmdown.hasPassed(5000)) {
    fsm.trigger(EVENT_GOT_HAPPY);
  }
}


State stressed("stressed", &on_stressed_enter, &on_stressed_loop, &on_stressed_leave);

// ///////////////////////////////////////////////////////////////////////////////////
// ON PARANOID
// ///////////////////////////////////////////////////////////////////////////////////
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
}

void on_paranoid_loop() {
  fast.update();
  shaker.on(500);
  shaker.update();

  // spin randomly
  if (random(2)<1) {
    kinetics.turn_right(360);
  } else {
    kinetics.turn_left(360);
  }

  // run kinetics loop
  kinetics.update();

  // continue in this state or calmdown?
  if(eyes.absdiff() > threshold) {
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

  kinetics.init();

  // state transition configuration
  fsm.add_timed_transition(&calibrating, &demo, 5000, NULL);
  fsm.add_timed_transition(&demo, &happy, 25000, NULL);
  fsm.add_transition(&happy, &stressed, EVENT_GOT_STRESSED, &on_trans_happy_to_stressed);
  fsm.add_transition(&stressed, &happy, EVENT_GOT_HAPPY, &on_trans_stressed_to_happy);
  fsm.add_transition(&stressed, &paranoid, EVENT_GOT_PARANOID, &on_trans_stressed_to_paranoid);
  fsm.add_transition(&paranoid, &happy, EVENT_GOT_HAPPY, NULL);
}


void bot_loop() {
  eyes.read();
  eyes.debug();
  Serial.print(",");

  kinetics.update();

//  Serial.print(", ");
//  Serial.println(threshold);
}

void loop() {
  bot_loop();
  fsm.run_machine();
}

