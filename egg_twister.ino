/**
 * eggs-twister is a simple periodic signal generator
 *
 * |<----------------PERIOD----------------->|
 * |<------Phase A------------>|<--Phase B-->|
 * A___________________________B_____________A
 *
 * Phase A - output level is low
 * Phase B - output level is high
 *
 * Screen:
 * |*Period_*Impulse|
 * | min:120<ds:299>|
 */

#include "AsyncDelay.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>

LiquidCrystal  lcd(7, 6, 5, 4, 3, 2);
AsyncDelay base_timer, phase_timer, autosave_timer;
AsyncDelay key_timer;
AsyncDelay up_per_timer, down_per_timer, up_off_timer, down_off_timer;
AsyncDelay up_per_iter_timer, down_per_iter_timer, up_off_iter_timer, down_off_iter_timer;
unsigned long period = PERIOD;
unsigned long pulse = PULSE_INTERVAL;
unsigned long offset = PERIOD - PULSE_INTERVAL;
int f_up_period = false;
int f_down_period = false;
int f_up_offset = false;
int f_down_offset = false;
int f_up_period_long = false;
int f_down_period_long = false;
int f_up_offset_long = false;
int f_down_offset_long = false;
int keyval = 10000;
int keyval_old = 10000;
int f_check_key = false;
int f_restart_period = false;
int f_autosave = false;
int f_phaseB = false;

void phaseA() {
  DPRINT("phaseA(): millis = ");
  DPRINTLN( millis() );
  digitalWrite(PIN_OUT_1,         LOW);
  f_phaseB = false;
  lcd.setCursor(8, 1);
  lcd.print(" ");
  lcd.setCursor(15, 1);
  lcd.print(" ");
}

void phaseB() {
  DPRINT("phaseB(): millis = ");
  DPRINTLN( millis() );
  digitalWrite(PIN_OUT_1,         HIGH);
  f_phaseB = true;
  lcd.setCursor(8, 1);
  lcd.print("<");
  lcd.setCursor(15, 1);
  lcd.print(">");
}

void dirtyPeriod() {
  lcd.setCursor(0, 0);
  lcd.print("*");
  f_restart_period = true;
  f_autosave = true;
  autosave_timer.start(3000);
}

void cleanPeriod() {
  lcd.setCursor(0, 0);
  lcd.print(" ");
}

void dirtyPulse() {
  lcd.setCursor(8, 0);
  lcd.print("*");
  f_restart_period = true;
  f_autosave = true;
  autosave_timer.start(3000);
}

void cleanPulse() {
  lcd.setCursor(8, 0);
  lcd.print(" ");
}

void updatePeriod() {
  lcd.setCursor(5, 1);
  lcd.print("   ");
  lcd.setCursor(5, 1);
  lcd.print(period/1000/60);
}

void updatePulse() {
  lcd.setCursor(12, 1);
  lcd.print("   ");
  lcd.setCursor(12, 1);
  lcd.print(pulse/100);
}

void upPeriod() {
  period += PERIOD_STEP;
  if(period > MAX_PERIOD) period = MAX_PERIOD;
  offset = period - pulse;
  DPRINT("period, min = ");
  DPRINTLN(period/60/1000);
  DPRINT("offset, ms = ");
  DPRINTLN(offset);
  dirtyPeriod();
  updatePeriod();
}

void downPeriod() {
  if (period > PERIOD_STEP) period -= PERIOD_STEP;
  else period = PERIOD_STEP;
  if (period < PERIOD_STEP) period = PERIOD_STEP;
  offset = period - pulse;
  DPRINT("period, min = ");
  DPRINTLN(period/60/1000);
  DPRINT("offset, ms = ");
  DPRINTLN(offset);
  dirtyPeriod();
  updatePeriod();
}

void upOffset() {
  pulse += OFFSET_STEP;
  if(pulse > MAX_PULSE_INTERVAL) pulse = MAX_PULSE_INTERVAL;
  offset = period - pulse;
  DPRINT("pulse, ms = ");
  DPRINTLN(pulse);
  DPRINT("offset, ms = ");
  DPRINTLN(offset);
  dirtyPulse();
  updatePulse();
}

void downOffset() {
  if(pulse > OFFSET_STEP) pulse -= OFFSET_STEP;
  else pulse = OFFSET_STEP;
  if(pulse < OFFSET_STEP) pulse = OFFSET_STEP;
  offset = period - pulse;
  DPRINT("pulse, ms = ");
  DPRINTLN(pulse);
  DPRINT("offset, ms = ");
  DPRINTLN(offset);
  dirtyPulse();
  updatePulse();
}

void butAllFree() {
  f_up_period = false;
  f_down_period = false;
  f_up_offset = false;
  f_down_offset = false;
  f_up_period_long = false;
  f_down_period_long = false;
  f_up_offset_long = false;
  f_down_offset_long = false;
  up_per_timer.stop();
  down_per_timer.stop();
  up_off_timer.stop();
  down_off_timer.stop();
  up_per_iter_timer.stop();
  down_per_iter_timer.stop();
  up_off_iter_timer.stop();
  down_off_iter_timer.stop();
}

// EEPROM read
unsigned long EEPROM_ulong_read(int addr) {    
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  unsigned long &num = (unsigned long&)raw;
  return num;
}

// EEPROM write
void EEPROM_ulong_write(int addr, unsigned long num) {
  byte raw[4];
  (unsigned long&)raw = num;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}

void saveSP() {
  EEPROM_ulong_write(0, period);
  EEPROM_ulong_write(4, pulse);
  Serial.println("Setpoints saved:");
  DPRINT("  period, min = ");
  DPRINTLN(period/60/1000);
  DPRINT("  pulse, ms  = ");
  DPRINTLN(pulse);
  cleanPeriod();
  cleanPulse();
}

void butDecode(int val) {
  butAllFree();
  if(val < 100)      f_down_period = true; // left
  else if(val < 250) f_up_offset = true;   // top
  else if(val < 400) f_down_offset = true; // bottom
  else if(val < 600) f_up_period = true;   // right
  else if(val < 900) saveSP();             // right right
}

void setup() {
  lcd.begin(16, 2);
  lcd.print(" Period  Impulse");
  lcd.setCursor(0, 1);
  lcd.print(" min:    ds:    ");
  Serial.begin(9600);
  pinMode(PIN_OUT_1,              OUTPUT);
  //pinMode(PIN_OUT_2,              OUTPUT);
  //pinMode(PIN_OUT_3,              OUTPUT);
  //pinMode(PIN_OUT_4,              OUTPUT);
  digitalWrite(PIN_OUT_1,         LOW);
  //digitalWrite(PIN_OUT_2,         LOW);
  //digitalWrite(PIN_OUT_3,         LOW);
  //digitalWrite(PIN_OUT_4,         LOW);
  delay(2000);
  
  unsigned long tmp_period = EEPROM_ulong_read(0);
  unsigned long tmp_pulse = EEPROM_ulong_read(4);
  if(tmp_period && tmp_pulse) {
    period = tmp_period;
    pulse = tmp_pulse;
    offset = period - pulse;
  }
  DPRINT("setup(): period, min = ");
  DPRINTLN(period/60/1000);
  DPRINT("setup(): pulse, ms = ");
  DPRINTLN(pulse);
  DPRINT("setup(): offset, ms = ");
  DPRINTLN(offset);
  updatePeriod();
  updatePulse();
}

void loop() {
  // check if it's need to restart base timer
  if(f_restart_period) {
    base_timer.stop();
    f_restart_period = false;
  }
  
  // check autosave
  if( f_autosave && autosave_timer.isExpired() ) {
    f_autosave = false;
    saveSP();
  }
  
  // check and set actual phase
  if( base_timer.isExpired() ) {
    phaseA();
    base_timer.start(period);
    phase_timer.start(offset);
  }
  else if( phase_timer.isExpired() && !f_phaseB ) {
    phaseB();
  }
  
  //serve button short press
  if(f_up_period && !f_up_period_long) {
    DPRINTLN("Button pressed: PERIOD+");
    f_up_period_long = true;
    upPeriod();
    up_per_timer.start(LONG_PRESS_INTERVAL);
  }
  if(f_down_period && !f_down_period_long) {
    DPRINTLN("Button pressed: PERIOD-");
    f_down_period_long = true;
    downPeriod();
    down_per_timer.start(LONG_PRESS_INTERVAL);
  }
  if(f_up_offset && !f_up_offset_long) {
    DPRINTLN("Button pressed: OFFSET+");
    f_up_offset_long = true;
    upOffset();
    up_off_timer.start(LONG_PRESS_INTERVAL);
  }
  if(f_down_offset && !f_down_offset_long) {
    DPRINTLN("Button pressed: OFFSET-");
    f_down_offset_long = true;
    downOffset();
    down_off_timer.start(LONG_PRESS_INTERVAL);
  }
  
  //serve button long press
  if( f_up_period_long && up_per_timer.isExpired() && up_per_iter_timer.isExpired() ) {
    DPRINTLN("Button pressed: PERIOD++");
    upPeriod();
    up_per_iter_timer.start(LONG_PRESS_ITERATION);
  }
  if( f_down_period_long && down_per_timer.isExpired() && down_per_iter_timer.isExpired() ) {
    DPRINTLN("Button pressed: PERIOD--");
    downPeriod();
    down_per_iter_timer.start(LONG_PRESS_ITERATION);
  }
  if( f_up_offset_long && up_off_timer.isExpired() && up_off_iter_timer.isExpired() ) {
    DPRINTLN("Button pressed: OFFSET++");
    upOffset();
    up_off_iter_timer.start(LONG_PRESS_ITERATION);
  }
  if( f_down_offset_long && down_off_timer.isExpired() && down_off_iter_timer.isExpired() ) {
    DPRINTLN("Button pressed: OFFSET--");
    downOffset();
    down_off_iter_timer.start(LONG_PRESS_ITERATION);
  }
  
  //read keys from KEYES
  if(!f_check_key) {
    int tmp;
    keyval = analogRead(PIN_KEYES);
    tmp = keyval - keyval_old;
    if( abs(tmp) > HYSTERESIS ) {
      f_check_key = true;
      key_timer.start(50); //
    }
  }
  else if( key_timer.isExpired() ) {
    int tmp;
    keyval = analogRead(PIN_KEYES);
    tmp = keyval - keyval_old;
    if( abs(tmp) > HYSTERESIS ) {
      keyval_old = keyval;
      f_check_key = false;
      key_timer.stop();
      butDecode(keyval);
    }
  }
}
