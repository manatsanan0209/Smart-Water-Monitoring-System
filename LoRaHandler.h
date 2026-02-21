#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <Arduino.h>

class LoRaHandler {
  public:
    LoRaHandler();
    void begin();
    void loop(); 
    
    // รับค่าทั้งหมดมาเก็บไว้ก่อน (ยังไม่ส่งทันที)
    void sendSensorData(float temp, bool tempStatus, float ph, bool phStatus, float doVal, bool doStatus);
    
    String getStatus() { return _status; }
    int getRSSI() { return _rssi; }
    String getLastSentTime() { return _lastSentTime; }

  private:
    void sendAT(String cmd);
    void sendSinglePacket(uint8_t type, uint8_t status, float value);
    
    String _status = "Disconnected";
    int _rssi = -999;
    String _lastSentTime = "-";
    unsigned long _lastTxMs = 0; // เวลาที่ส่งครั้งล่าสุด (ใช้โชว์บนจอ)

    // --- ส่วนที่เพิ่มใหม่: ตัวแปรสำหรับระบบคิวส่งต่อเนื่อง ---
    int _sendState = 0; // 0=Idle, 1=SendTemp, 2=WaitPH, 3=SendPH, 4=WaitDO, 5=SendDO
    unsigned long _stateTimer = 0; // จับเวลาแต่ละขั้นตอน
    
    // ตัวแปรพักค่าข้อมูล (Buffer)
    float _bufTemp, _bufPh, _bufDo;
    bool _stTemp, _stPh, _stDo;
};

#endif