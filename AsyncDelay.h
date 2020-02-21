#ifndef ASYNCDELAY_H
#define ASYNCDELAY_H

#include <Arduino.h>
#include "defines.h"

class AsyncDelay{
  unsigned long timer;
  unsigned long start_millis;
  bool state;//running -> true, idle -> false
 public:
  AsyncDelay(void);
  void start(unsigned long interval);
  void stop(void);
  bool isExpired(void);
};

#endif
