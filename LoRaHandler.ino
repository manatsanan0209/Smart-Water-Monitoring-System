#include "LoRaHandler.h"
#include <SoftwareSerial.h>

SoftwareSerial LoRaSerial(7, 6); 

LoRaHandler::LoRaHandler() { }

void LoRaHandler::begin() {
  LoRaSerial.begin(9600); 
  delay(1000);
  sendAT("AT");
  delay(100);
  
  _status = "Joining..."; 
  Serial.println("[LoRa] System Init & Joining TTN...");
  sendAT("AT+JOIN"); 
}

void LoRaHandler::loop() {
  // 1. อ่านค่าตอบกลับจาก LoRa (เหมือนเดิม)
  while (LoRaSerial.available()) {
    String line = LoRaSerial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      Serial.print("[LoRa RX] "); Serial.println(line); 
      
      if (line.indexOf("Network joined") != -1 || line.indexOf("Joined") != -1) _status = "Joined";
      else if (line.indexOf("Join failed") != -1) _status = "Failed";
      
      if (line.indexOf("RSSI") != -1) {
         int idx = line.indexOf("RSSI");
         _rssi = line.substring(idx + 5).toInt();
      }
      
      if (line.indexOf("Done") != -1 && _status == "Joined") {
         unsigned long diff = (millis() - _lastTxMs) / 1000;
         _lastSentTime = String(diff) + "s ago"; 
      }
    }
  }

  // 2. อัปเดตเวลาโชว์
  if (_lastTxMs > 0) {
    unsigned long sec = (millis() - _lastTxMs) / 1000;
    _lastSentTime = String(sec) + "s ago";
  }

  // --- 3. ระบบจัดการคิวส่ง (State Machine) ---
  // ส่ง Temp -> รอ 4วิ -> ส่ง pH -> รอ 4วิ -> ส่ง DO -> จบ
  if (_sendState > 0) {
    if (millis() - _stateTimer >= 4000) { // เว้นระยะ 4 วินาทีต่อแพ็กเกจ
       _stateTimer = millis();
       
       switch (_sendState) {
         case 1: // ถึงคิวส่ง Temp
           Serial.println(">>> Burst 1/3: Sending Temperature");
           sendSinglePacket(0x01, _stTemp ? 1:0, _bufTemp);
           _sendState = 2; // ไปสถานะรอ
           break;
           
         case 2: // ถึงคิวส่ง pH
           Serial.println(">>> Burst 2/3: Sending pH");
           sendSinglePacket(0x02, _stPh ? 1:0, _bufPh);
           _sendState = 3; // ไปสถานะรอ
           break;
           
         case 3: // ถึงคิวส่ง DO
           Serial.println(">>> Burst 3/3: Sending DO");
           sendSinglePacket(0x03, _stDo ? 1:0, _bufDo);
           _sendState = 0; // จบภารกิจ กลับสู่โหมด Idle
           break;
       }
    }
  }
}

// ฟังก์ชันนี้จะแค่ "รับคำสั่ง" แล้วเริ่มกระบวนการ (ไม่ส่งเองทันที)
void LoRaHandler::sendSensorData(float temp, bool tempStatus, float ph, bool phStatus, float doVal, bool doStatus) {
  // 1. เก็บค่าลงตัวแปรพัก
  _bufTemp = temp; _stTemp = tempStatus;
  _bufPh   = ph;   _stPh   = phStatus;
  _bufDo   = doVal; _stDo  = doStatus;
  
  // 2. เริ่มต้นกระบวนการส่ง (Trigger State Machine)
  if (_sendState == 0) {
     _sendState = 1;      // เริ่มที่ Temp
     _stateTimer = millis() - 4000; // ลบเวลาเพื่อให้ Packet แรกส่งทันทีโดยไม่ต้องรอ
  }
}

// ส่ง Packet เดี่ยว (รูปแบบ 6 Bytes: Ver,Evt,Type,Status,ValH,ValL)
void LoRaHandler::sendSinglePacket(uint8_t type, uint8_t status, float value) {
  uint8_t version = 0x00;
  uint8_t event   = 0x01;
  int16_t valInt  = (int16_t)(value * 100);

  char payload[13]; 
  sprintf(payload, "%02X%02X%02X%02X%04X", version, event, type, status, valInt);
  
  String cmd = "AT+MSGHEX=\"" + String(payload) + "\"";
  sendAT(cmd);
  _lastTxMs = millis(); 
}

void LoRaHandler::sendAT(String cmd) {
  LoRaSerial.println(cmd);
}