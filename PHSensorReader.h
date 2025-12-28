#ifndef PH_SENSOR_READER_H
#define PH_SENSOR_READER_H

#include <Arduino.h>
#include <ModbusMaster.h>

// Structure สำหรับเก็บค่าที่อ่านได้ เพื่อส่งกลับไปทีเดียว
struct SensorResult {
  float pH;
  float temp;
  uint8_t errorCode; // 0 = Success
  String errorMsg;
};

class PHSensorReader {
  public:
    // Constructor: รับค่าขา DE/RE Pin
    PHSensorReader(uint8_t dePin);
    
    // เริ่มต้นการทำงาน (Setup Serial)
    void begin();
    
    // สั่งอ่านค่าจาก Sensor
    SensorResult read();

  private:
    uint8_t _dePin;
    ModbusMaster _node;
    
    // Helper function แปลง Error code เป็นข้อความ
    String getErrorString(uint8_t code);
};

#endif