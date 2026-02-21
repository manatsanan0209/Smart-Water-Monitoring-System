#include "PHSensorReader.h"

uint8_t _globalDEPin;

void preTransmission() {
  digitalWrite(_globalDEPin, HIGH);
  delayMicroseconds(100);
}

void postTransmission() {
  delayMicroseconds(300);
  digitalWrite(_globalDEPin, LOW);
}

PHSensorReader::PHSensorReader(uint8_t dePin) {
  _dePin = dePin;
  _globalDEPin = dePin;
}

void PHSensorReader::begin() {
  pinMode(_dePin, OUTPUT);
  digitalWrite(_dePin, LOW);

  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(9600);

  _node.begin(12, Serial1);
  _node.preTransmission(preTransmission);
  _node.postTransmission(postTransmission);
}

SensorResult PHSensorReader::read() {
  SensorResult result;
  
  uint8_t resCode = _node.readInputRegisters(0x0000, 2);
  result.errorCode = resCode;

  if (resCode == ModbusMaster::ku8MBSuccess) {
    uint16_t rawTemp = _node.getResponseBuffer(0);
    uint16_t rawPh = _node.getResponseBuffer(1);
    
    result.temp = rawTemp / 100.0;
    result.pH = rawPh / 100.0;
    result.errorMsg = "Success";
  } else {
    result.temp = -99.0;
    result.pH = -99.0;
    result.errorMsg = getErrorString(resCode);
  }

  return result;
}

String PHSensorReader::getErrorString(uint8_t code) {
  switch (code) {
    case ModbusMaster::ku8MBSuccess: return "Success";
    case ModbusMaster::ku8MBInvalidSlaveID: return "Invalid Slave ID";
    case ModbusMaster::ku8MBInvalidFunction: return "Invalid Function";
    case ModbusMaster::ku8MBResponseTimedOut: return "Response Timed Out";
    case ModbusMaster::ku8MBInvalidCRC: return "Invalid CRC";
    default: return "Unknown Error";
  }
}