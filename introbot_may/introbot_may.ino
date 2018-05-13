#include "statemachine.h"
#include "flasher.h"
#include "config.h"

#include "kinetics.h"
#include "eyes.h"
#include "shaker.h"

#define LED_PIN 10

#define EVENT_GOT_READY  900;
#define EVENT_GOT_SCARED 1000
#define EVENT_GOT_SLEEPY 1001


Eyes eyes(LDR_LEFT, LDR_RIGHT);
Shaker shaker(PIN_SHAKER);
Kinetics kinetics;

int threshold = 8;

Flasher superfast(LED_PIN, 0.4);
Flasher slow(LED_PIN, 0.009);
Flasher fast(LED_PIN, 0.06);

// 0.009 nice and smooth
// 0.08 fast hearbeat
// 0.4 fast flashing

Chrono calmdown;

// ON CALIBRATING
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

// ON DEMO
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

// ON SLEEPING
void on_sleeping_enter() {
  Serial.println("on_sleeping_enter");
  slow.flash();
}

void on_sleeping_leave() {
  Serial.println("on_sleeping_leave");
  slow.stop();
}

void on_sleeping_loop() {
  slow.update();
//  if(mavg.get() > threshold) {
//      fsm.trigger(EVENT_GOT_SCARED);
//  }
}

State sleeping("sleeping", &on_sleeping_enter, &on_sleeping_loop, &on_sleeping_leave);

void on_stressed_enter() {
  Serial.println("on_stressed_enter");
  fast.flash();
}

void on_stressed_leave() {
  Serial.println("on_stressed_leave");
  fast.stop();
}

void on_stressed_loop() {
//  Serial.println("on_stressed_loop");
  fast.update();

//  if(mavg.get() > threshold) {
//    calmdown.restart();
//  }

  if(calmdown.hasPassed(5000)) {
      fsm.trigger(EVENT_GOT_SLEEPY);
  }
}


State stressed("stressed", &on_stressed_enter, &on_stressed_loop, &on_stressed_leave);

void on_trans_leeping_to_stressed()
{
  Serial.println("Transitioning from SLEEPY to STRESSED");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  kinetics.init();
  
  fsm.add_timed_transition(&calibrating, &demo, 5000, NULL);
  fsm.add_timed_transition(&demo, &sleeping, 25000, NULL);
  fsm.add_transition(&sleeping, &stressed, EVENT_GOT_SCARED, &on_trans_leeping_to_stressed);
  fsm.add_transition(&stressed, &sleeping, EVENT_GOT_SLEEPY, NULL);
}


void event_loop() {
  kinetics.update();
//  eyes.read_eyes();
//  eyes.debug();
//
//  Serial.print(", ");
//  Serial.println(threshold);
}

void loop() {
  event_loop();
  fsm.run_machine();
}

