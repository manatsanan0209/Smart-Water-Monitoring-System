#ifndef PH_SENSOR_READER_H
#define PH_SENSOR_READER_H

#include <Arduino.h>
#include <ModbusMaster.h>

struct SensorResult {
  float pH;
  float temp;
  uint8_t errorCode;
  String errorMsg;
};

class PHSensorReader {
  public:
    PHSensorReader(uint8_t dePin);
    
    void begin();
    
    SensorResult read();

  private:
    uint8_t _dePin;
    ModbusMaster _node;
    
    String getErrorString(uint8_t code);
};

#endif