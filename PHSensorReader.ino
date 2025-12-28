#include "PHSensorReader.h"

// จำเป็นต้องประกาศตัวแปร Global หรือ Static สำหรับ Callback ของ ModbusMaster
uint8_t _globalDEPin;

void preTransmission() {
  digitalWrite(_globalDEPin, HIGH);
  delayMicroseconds(100);
}

void postTransmission() {
  delayMicroseconds(300);
  digitalWrite(_globalDEPin, LOW);
}

// --- Constructor ---
PHSensorReader::PHSensorReader(uint8_t dePin) {
  _dePin = dePin;
  _globalDEPin = dePin; // ส่งค่าให้ Global เพื่อใช้ใน Callback
}

void PHSensorReader::begin() {
  pinMode(_dePin, OUTPUT);
  digitalWrite(_dePin, LOW);

  // ตั้งค่า Serial1 สำหรับ Pico (Tx=0, Rx=1)
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(9600);

  // ตั้งค่า Modbus ID 12
  _node.begin(12, Serial1);
  _node.preTransmission(preTransmission);
  _node.postTransmission(postTransmission);
}

SensorResult PHSensorReader::read() {
  SensorResult result;
  
  // อ่าน 2 Registers เริ่มจาก 0x0000
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