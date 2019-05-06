#include "KBusService.h"
#include "SoftwareSerialParity.h"

//Constructor
KBusService::KBusService() {
  bus -> begin(9600, 2); //EVEN parity
}

BusResponse KBusService::getData(byte request[]) {
  //clear bus receive buffer;
  while(bus -> read() >= 0);
  
  bus -> write(request, request[1]);

  uint8_t expectedInputSize = request[2];
  byte responseBytes[expectedInputSize] = {};
  byte checkSum = 0;

  unsigned long startTime = millis();
  bool timeoutError = false;

  while(!timeoutError && bus -> available() < expectedInputSize) {
    if (millis() - startTime > 50) {
      timeoutError = true;
    }
  }

  if (timeoutError) {
    while(bus -> read() >= 0);
    return {.data=0, .status=1};
  }

  uint8_t i = 0;
  int checkSumIndex = expectedInputSize - 1;
  
  while(i < expectedInputSize) {
    byte b = bus -> read();
    responseBytes[i] = b;
    if (i < checkSumIndex) checkSum ^= b;
    i++;
  }

  float data = responseBytes[3];
  int status = checkSum == responseBytes[checkSumIndex] ? 0 : 1;

  return {.data=data, .status=status};
}

BusResponse KBusService::getEngineTemp() {
  BusResponse response = getData(tempRequest);
  response.data = response.data * 0.75 - 48;
  return response;
}

BusResponse KBusService::getCarSpeed() {
  BusResponse response = getData(speedRequest);
  response.data = response.data * 1.112;
  return response;
}
