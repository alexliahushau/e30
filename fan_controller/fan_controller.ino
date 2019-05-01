#include "KBusService.h"

#define FAN_FREQ_OUTPUT 9
#define PROFILE "qa"

const String TAB = "\t";
float temp = 0;
int speed = 0;
int val = 19863; // 100Hz
byte t_low = 85;
byte t_high = 98;
byte fan_load = 0;
float duty = 0;

KBusService kbus (2, 4);

void setup () {
  if (PROFILE == "qa") Serial.begin(9600);
  pinMode (FAN_FREQ_OUTPUT, OUTPUT) ;
  
  TCCR1A = 0xE2 ;    // pins 9 and 10 in antiphase, mode 14 = fast 16 bit
  TCCR1B = 0x1A ;    // clock divide by 8
  TCNT1 = 0x0000 ;
  ICR1 = val;
  OCR1A = val;
}

void loop () {
  getTempData();
  getSpeedData();
  fanController();
  
  if (PROFILE == "prod") {
    delay(1000); 
  }
}

void fanController() {
  if (temp <= t_low) {
    fan_load = 0;
    setFanLoad();
    debug("STOP_FAN");
    return;
  }

  //To prevent out of range after mapping
  if (temp >= t_high) {
    fan_load = 100;
    setFanLoad();
    debug("FAN_MAX_RPM");
    return;
  }

  fan_load = map(temp, t_low , t_high, 1, 99);
  setFanLoad();
  debug();
}

void getTempData() {
  BusResponse resp = kbus.getEngineTemp();

  if (resp.status == 0) {
    temp = resp.data;
    debug();
    return;
  }

  temp = 120;
}

void getSpeedData() {
  BusResponse resp = kbus.getCarSpeed();

  if (resp.status == 0) {
    speed = resp.data;
    debug();
    return;
  }

  temp = 0;
}

void setFanLoad() {
  /** Map fan load % to voltage controller input signal from 91(0)..6(100) % */
  duty = map(fan_load, 0, 100, 91, 6);
  OCR1A = val * (1 - duty / 100);
}

void debug() {
  debug("");
}

void debug(String msg) {
  if (PROFILE == "prod") return;
  String debug_str = String("temp: " + String(temp) + TAB + "speed: " + String(speed) + TAB + "fan_load: " + fan_load + TAB + "duty: " + duty + TAB + msg);
  Serial.println(debug_str);
}
