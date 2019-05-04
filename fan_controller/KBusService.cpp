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

  byte responseBytes[] = {};
  byte checkSum = 0;
  uint8_t expectedInputSize = request[2];
  unsigned long startTime = millis();
  bool timeoutError = false;

  while(!timeoutError && bus -> available() < expectedInputSize) {
    if (millis() - startTime > 400) {
      timeoutError = true;
    }
  }

  BusResponse response;

  if (timeoutError) {
    response.data = 0;
    response.status = 1;

    return response;
  }

  uint8_t inputSize = 0;
  
  while(inputSize < expectedInputSize) {
    byte b = bus -> read();
    responseBytes[inputSize] = b;
    checkSum ^b;
    inputSize++;
  }

  response.data = responseBytes[3];
  response.status = checkSum == responseBytes[5] ? 0 : 1;

  return response;
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
