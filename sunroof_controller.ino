/********************************************************************************************
*****  Temp input from ecu temp sensor (white): map temp=[-40...120] to voltage=[5...0] *****
*****  |-----------|---------|--------------|                                           *****
*****  |  PROFILE  |  DEBUG  |  LOOP_DELAY  |                                           *****
*****  |------------------------------------|                                           *****
*****  |    prod   |  false  |     true     |                                           *****
*****  |     qa    |  true   |     false    |                                           *****
*****  |-----------|---------|--------------|                                           *****
*********************************************************************************************/

#define SWITCH_INPUT_1 A3
#define SWITCH_INPUT_2 A4
#define SWITCH_INPUT_3 A5

#define CLOSE_SENSOR_INPUT A6

#define OPEN_OUTPUT 8
#define CLOSE_OUTPUT 9

#define PROFILE "qa"  

String TAB = "\t";
boolean OPEN_FULL = false;
boolean CLOSE_FULL = false;
boolean TOGGLE_UP = 

void setup () {
  if (PROFILE == "qa") Serial.begin(9600);
  pinMode (OPEN_OUTPUT 8, OUTPUT);
  pinMode (CLOSE_OUTPUT 9, OUTPUT);
}

void loop () {
  sunroofController(); 
}

void sunroofController() {
  if (fuse()) {
    reset();
    return;
  }
  
  if (CLOSE_FULL && analogRead(CLOSE_SENSOR)) {
     reset();
     return;        
  }
  
  if (_open()) {
    digitalWrite(CLOSE_OUTPUT, LOW);
    digitalWrite(OPEN_OUTPUT, HIGH);
    return;    
  }
  
  if (_close()) {
    digitalWrite(CLOSE_OUTPUT, HIGH);
    digitalWrite(OPEN_OUTPUT, LOW);
    return;   
  }
  
  if (openFull()) {
    OPEN_FULL = true;
    digitalWrite(CLOSE_OUTPUT, LOW);
    digitalWrite(OPEN_OUTPUT, HIGH);
    return;    
  }
  
  if (_closeFull()) {
    CLOSE_FULL = true;
    digitalWrite(CLOSE_OUTPUT, HIGH);
    digitalWrite(OPEN_OUTPUT, LOW);
    return;    
  }
  
  if (toggleUp()) {
            
  }
  
}

void reset() {
  OPEN_FULL = false;
  CLOSE_FULL = false;
  digitalWrite(CLOSE_OUTPUT, LOW);
  digitalWrite(OPEN_OUTPUT, LOW);
}

boolean openFull() {
  return OPEN_FULL || (SWITCH_INPUT_3 && SWITCH_INPUT_4);
}

boolean closeFull() {
  return CLOSE_FULL || (SWITCH_INPUT_3 && SWITCH_INPUT_5);
}

boolean _open() {
  return SWITCH_INPUT_4 && !(SWITCH_INPUT_3 || SWITCH_INPUT_5);
}

boolean _close() {
  return SWITCH_INPUT_5 && !(SWITCH_INPUT_3 || SWITCH_INPUT_4);
}

boolean toggle_up() {
  return SWITCH_INPUT_3 && !(SWITCH_INPUT_4 || SWITCH_INPUT_5);
}

boolean fuse() {
  return false;//TODO
}


void debug() {
  debug("");
}

void debug(String msg) {
  if (PROFILE == "prod") return;
  String debug_str = String("temp: " + temp + TAB + "fan_load: " + fan_load + TAB + "duty: " + duty + TAB + "voltage: " + temp_sensor_voltage + TAB + msg);
  Serial.println(debug_str);
}
