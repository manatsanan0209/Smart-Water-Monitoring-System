#include "DOSensorReader.h"

// ใช้ชื่อตัวแปรต่างจาก PHSensorReader เพื่อป้องกันการชนกัน (Duplicate definition)
uint8_t _globalDEPin_DO; 

void preTransmissionDO() {
  digitalWrite(_globalDEPin_DO, HIGH);
  delayMicroseconds(100);
}

void postTransmissionDO() {
  delayMicroseconds(300);
  digitalWrite(_globalDEPin_DO, LOW);
}

DOSensorReader::DOSensorReader(uint8_t dePin) {
  _dePin = dePin;
  _globalDEPin_DO = dePin;
}

void DOSensorReader::begin() {
  pinMode(_dePin, OUTPUT);
  digitalWrite(_dePin, LOW);

  // Serial1 เริ่มต้นจาก Main หรือ PHSensorReader แล้ว แต่เรียกซ้ำเพื่อความชัวร์ได้
  Serial1.begin(9600);

  // ตั้งค่า Modbus ID 1 สำหรับ DO Sensor
  _node.begin(1, Serial1);
  _node.preTransmission(preTransmissionDO);
  _node.postTransmission(postTransmissionDO);
}

float DOSensorReader::convertToFloat(uint16_t high, uint16_t low) {
  uint32_t combined = ((uint32_t)high << 16) | low;
  float f;
  memcpy(&f, &combined, sizeof(f));
  return f;
}

DOSensorResult DOSensorReader::read() {
  DOSensorResult result;
  
  // ค่า Default หากอ่านไม่เจอ
  result.DO = 0.0;
  result.temp = 0.0;
  result.saturation = 0.0;
  
  // สั่งอ่าน Holding Register (0x03) จำนวน 6 Register เริ่มที่ 0x0000
  uint8_t resCode = _node.readHoldingRegisters(0x0000, 6);
  result.errorCode = resCode;

  if (resCode == ModbusMaster::ku8MBSuccess) {
    // Register 0-1: Saturation
    result.saturation = convertToFloat(_node.getResponseBuffer(0), _node.getResponseBuffer(1));
    // Register 2-3: DO (mg/L)
    result.DO = convertToFloat(_node.getResponseBuffer(2), _node.getResponseBuffer(3));
    // Register 4-5: Temp
    result.temp = convertToFloat(_node.getResponseBuffer(4), _node.getResponseBuffer(5));
    
    result.errorMsg = "Success";
  } else {
    result.DO = -1.0; // ค่าแสดง Error
    result.errorMsg = getErrorString(resCode);
  }

  return result;
}

String DOSensorReader::getErrorString(uint8_t code) {
  switch (code) {
    case ModbusMaster::ku8MBSuccess: return "Success";
    case ModbusMaster::ku8MBInvalidSlaveID: return "Invalid Slave ID";
    case ModbusMaster::ku8MBInvalidFunction: return "Invalid Function";
    case ModbusMaster::ku8MBResponseTimedOut: return "Response Timed Out";
    case ModbusMaster::ku8MBInvalidCRC: return "Invalid CRC";
    default: return "Unknown Error";
  }
}