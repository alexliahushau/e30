#include "KBusService.h"
#include "SoftwareSerialParity.h"

//Constructor
KBusService::KBusService(uint8_t receivePin, uint8_t transmitPin) {
  bus = SoftwareSerialParity(receivePin, transmitPin);
  bus.begin(9600, 2); //EVEN parity
}

BusResponse KBusService::getData(byte &request) {
  bus.write(request, 6);
  bus.flush();

  byte responseBytes[] = {};
  byte checkSum = 0;
  uint8_t inputSize = 0;
  unsigned long startTime = millis(); 
  
  while(bus.available() > 0 && inputSize < 6 && (millis() - startTime < 400)) {
    byte b = bus.read();
    checkSum ^b;
    inputSize++;
  }

  BusResponse response;
  
  response.data = responseBytes[3];
  response.status = inputSize == 6 && checkSum == responseBytes[5] ? 0 : 1;

  return response;
}

BusResponse KBusService::getEngineTemp() {
  BusResponse response = getData(tempRequest);

  if (response.data) {
    response.data = response.data * 0,75 - 48;
  }

  return response;
}

BusResponse KBusService::getCarSpeed() {
  BusResponse response = getData(speedRequest);

  if (response.data) {
    response.data = response.data * 1,112;
  }

  return response;
}
