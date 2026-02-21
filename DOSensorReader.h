#ifndef DO_SENSOR_READER_H
#define DO_SENSOR_READER_H

#include <Arduino.h>
#include <ModbusMaster.h>

struct DOSensorResult {
  float DO;
  float temp;
  float saturation;
  uint8_t errorCode;
  String errorMsg;
};

class DOSensorReader {
  public:
    DOSensorReader(uint8_t dePin);
    
    void begin();
    
    DOSensorResult read();

  private:
    uint8_t _dePin;
    ModbusMaster _node;
    
    float convertToFloat(uint16_t high, uint16_t low);
    String getErrorString(uint8_t code);
};

#endif