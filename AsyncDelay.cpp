#ifndef ASYNCDELAY_H
#include "AsyncDelay.h"
#endif

AsyncDelay::AsyncDelay() {
  state = false;
}

void AsyncDelay::start(unsigned long interval) {
  timer = interval;
  start_millis = millis();
  state = true;
  //DPRINTLN("Timer started");
}

void AsyncDelay::stop() {
  state = false;
}

bool AsyncDelay::isExpired() {
  unsigned long cur_millis;

  if (state == false) return true;
  cur_millis = millis();
  if ((cur_millis-start_millis) < timer){
    return false;
  } else {
    state = false;
    return true;
  }
}
