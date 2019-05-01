#include "SoftwareSerialParity.h"
#include <Arduino.h>

struct BusResponse {
  float data;
  int status; // 0 - success; 1 - failed  
};

class KBusService {

private:

  const byte tempRequest[9] =  {18, 9, 6, 0, 0,  0, 215, 2, 200};
  const byte speedRequest[9] = {18, 9, 6, 0, 0, 24, 227, 2, 228};

  SoftwareSerialParity &bus;  
  BusResponse getData(byte &request);

public:

  KBusService(uint8_t receivePin, uint8_t transmitPin);
  BusResponse getEngineTemp();
  BusResponse getCarSpeed();

};
