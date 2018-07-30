/********************************************************************************************
*****  Temp input from ecu temp sensor (white): map temp=[-40...120] to voltage=[5...0] *****
*****  |-----------|---------|--------------|                                           *****
*****  |  PROFILE  |  DEBUG  |  LOOP_DELAY  |                                           *****
*****  |------------------------------------|                                           *****
*****  |    prod   |  false  |     true     |                                           *****
*****  |     qa    |  true   |     false    |                                           *****
*****  |-----------|---------|--------------|                                           *****
*********************************************************************************************/

#define TEMP_INPUT A1
#define FAN_FREQ_OUTPUT 9
#define PROFILE "qa"  
#define NUM_READS 200
#define MAX_INPUT_VOLTAGE 5.1
#define TEMP_SENSOR_MIN -40
#define TEMP_SENSOR_MAX 120

String TAB = "\t";

int temp = 0;
float temp_sensor_voltage = 0;
int val = 19863; // 100Hz
byte t_low = 85;
byte t_high = 98;
byte fan_load = 0;
float duty = 0;

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
  filterTempInput();
  fanController();
  if (PROFILE == "prod") delay(1000); 
}

void fanController() {
  if (temp <= t_low) {
    fan_load = 0;
    setFanLoad();
    debug("LOW TEMP:\tSTOP_FAN");
    return;
  }

  //To prevent out of range after mapping
  if (temp >= t_high) {
    fan_load = 100;
    setFanLoad();
    debug("HIGH TEMP:\t FAN_MAX_RPM");
    return;
  }

  fan_load = map(temp, t_low , t_high, 1, 99);
  setFanLoad();
  debug();
}

void setFanLoad() {
  /** Map fan load % to voltage controller input signal from 91(0)..6(100) % */
  duty = map(fan_load, 0, 100, 91, 6);
  OCR1A = val * (1 - duty / 100);
}


int filterTempInput() {
  // read multiple values and sort them to take the mode
  float sortedValues[NUM_READS];
  for (int i = 0; i < NUM_READS; i++) {
    float value = analogRead(TEMP_INPUT);
    int j;
    if (value < sortedValues[0] || i == 0) {
      j = 0; //insert at first position
    }
    else {
      for (j = 1; j < i; j++) {
        if (sortedValues[j - 1] <= value && sortedValues[j] >= value) {
          // j is insert position
          break;
        }
      }
    }
    for (int k = i; k > j; k--) {
      // move all values higher than current reading up one position
      sortedValues[k] = sortedValues[k - 1];
    }
    sortedValues[j] = value; //insert current reading
  }
  
  //return scaled mode of 10 values
  float returnval = 0;
  for (int i = NUM_READS / 2 - 5; i < (NUM_READS / 2 + 5); i++) {
    returnval += sortedValues[i];
  }

  float t = round(returnval / 10);
  temp_sensor_voltage = t * MAX_INPUT_VOLTAGE / 1023;
  temp = map(t, 0, 1023, TEMP_SENSOR_MAX, TEMP_SENSOR_MIN);
}

void debug() {
  debug("");
}

void debug(String msg) {
  if (PROFILE == "prod") return;
  String debug_str = String("temp: " + temp + TAB + "fan_load: " + fan_load + TAB + "duty: " + duty + TAB + "voltage: " + temp_sensor_voltage + TAB + msg);
  Serial.println(debug_str);
}
