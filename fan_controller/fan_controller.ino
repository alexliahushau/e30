#include <avr/wdt.h>
#include "KBusService.h"

#define FAN_FREQ_OUTPUT 9
#define PROFILE "qa"

const String TAB = "\t";
float temp = 0;
int speed = 0;
int val = 19863; // 100Hz
byte t_low = 89;
byte t_high = 93;
byte fan_load = 0;
float duty = 0;

String errors = "";

KBusService kbus;

void setup () {
  Serial.begin(115200);

  //Watchdog setup
  Serial.println("Wait 5 sec..");
  delay(5000); // Задержка, чтобы было время перепрошить устройство в случае bootloop
  wdt_enable (WDTO_8S); // Для тестов не рекомендуется устанавливать значение менее 8 сек.
  Serial.println("Watchdog enabled.");
  
  pinMode (FAN_FREQ_OUTPUT, OUTPUT);
  
  TCCR1A = 0xE2 ;    // pins 9 and 10 in antiphase, mode 14 = fast 16 bit
  TCCR1B = 0x1A ;    // clock divide by 8
  TCNT1 = 0x0000 ;
  ICR1 = val;
  OCR1A = val;
}

void loop () {
  getTempData();
  
  delay(10);  // Looks like ECU requires a delay between requests;
  
  getSpeedData();
  fanController();

  log();
  
  delay(1000);
  wdt_reset(); //reset watchdog
}

void fanController() {
  if (temp <= t_low) {
    fan_load = 0;
    setFanLoad();
    return;
  }

  //To prevent out of range after mapping
  if (temp >= t_high) {
    fan_load = 100;
    setFanLoad();
    return;
  }

  fan_load = map(temp, t_low , t_high, 1, 99);
  setFanLoad();
}

void getTempData() {
  BusResponse resp = kbus.getEngineTemp();

  if (resp.status == 0) {
    temp = resp.data;
    return;
  }

  logError("temp error");
  temp = 120;
}

void getSpeedData() {
  BusResponse resp = kbus.getCarSpeed();

  if (resp.status == 0) {
    speed = resp.data;
    return;
  }
  logError("speed error");
  speed = 0;
}

void logError(String error) {
  if (errors == "") {
    errors = error;
    return;
  }

  errors = errors + ", " + error;
};

void setFanLoad() {
  /** Map fan load % to voltage controller input signal from 91(0)..6(100) % */
  duty = map(fan_load, 0, 100, 91, 6);
  OCR1A = val * (1 - duty / 100);
}

void log() {
  debug("");
}

void debug(String msg) {
  String err_part = (errors == "") ? "" : TAB + "ERRORS[" + errors + "]";
  String debug_str = String("temp: " + String(temp) + TAB + "speed: " + String(speed) + TAB + "fan_load: " + fan_load + TAB + msg + err_part);
  Serial.println(debug_str);
  errors = "";
}
