#ifndef DEFINES_H
#define DEFINES_H

////////
//Setup:
////////
#define DEBUG

#define PERIOD                         60000UL     // 5400000UL   // 90*60*1000  // 90min
#define MAX_PERIOD                     10800000UL  // 180*60*1000 // 180min
#define PULSE_INTERVAL                 12500UL     // 12.5s
#define MAX_PULSE_INTERVAL             30000UL     // 30s
#define LONG_PRESS_INTERVAL            500         // 0.5s
#define LONG_PRESS_ITERATION           100         // 0.1s per action
#define PERIOD_STEP                    60000       // 60*1000     // 1min
#define OFFSET_STEP                    100         // 0.1s
#define HYSTERESIS                     50
////////////
//Setup ends
////////////

#define PIN_KEYES                      A0
#define PIN_OUT_1                      8
//#define PIN_OUT_2                      3
//#define PIN_OUT_3                      4
//#define PIN_OUT_4                      5

#ifdef DEBUG
#define DPRINT(x) Serial.print(x)
#define DPRINTLN(x) Serial.println(x)
#else
#define DPRINT(x)
#define DPRINTLN(x)
#endif

#endif
