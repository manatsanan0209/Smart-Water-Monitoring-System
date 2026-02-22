#include "PHSensorReader.h"
#include "DOSensorReader.h"
#include "SystemGUI.h"
#include "LoRaHandler.h" // <--- 1. เพิ่ม Header นี้

const int PIN_RS485_DIR = 2; 
const int PIN_RESET_BTN = 15;
const int READ_INTERVAL = 2000; 

// กำหนดเวลาส่ง LoRa (20 วินาที = 20000 ms)
const unsigned long LORA_INTERVAL = 20000; 

PHSensorReader phSensor(PIN_RS485_DIR);
DOSensorReader doSensor(PIN_RS485_DIR);
SystemGUI gui;
LoRaHandler lora; // <--- 2. สร้างตัวแปร LoRa

unsigned long lastReadTime = 0;
unsigned long lastLoRaTime = 0; // <--- 3. ตัวนับเวลาสำหรับ LoRa

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RESET_BTN, INPUT_PULLUP);

  // เริ่มต้น Sensors
  phSensor.begin();
  doSensor.begin();
  
  // เริ่มต้น GUI
  gui.init();
  
  // <--- 4. เริ่มต้น LoRa (สั่ง Join)
  lora.begin(); 
  
  Serial.println("=== System Started (LoRa on SW Serial GP6/7) ===");
}

void loop() {
  gui.loop();
  lora.loop();

  // เช็คปุ่ม Reset
  if (digitalRead(PIN_RESET_BTN) == LOW) {
    Serial.println("!!! MANUAL RESET TRIGGERED !!!");
    gui.init();      
    delay(500);
    return;
  }

  // Loop อ่านค่า Sensor (ทุก 2 วินาที)
  if (millis() - lastReadTime >= READ_INTERVAL) {
    lastReadTime = millis();
    
    // --- 1. อ่านค่าจาก Sensor ---
    SensorResult phData = phSensor.read();
    delay(20); // เว้นจังหวะ RS485 (ลดจาก 100ms เพื่อไม่บล็อก touch)
    DOSensorResult doData = doSensor.read();

    // เลือกค่า Temp ที่จะใช้
    float systemTemp = (doData.errorCode == 0) ? doData.temp : phData.temp; 
    
    // แสดงผลออก Serial Monitor
    Serial.print("pH: "); Serial.print(phData.pH);
    Serial.print(" | DO: "); Serial.print(doData.DO);
    Serial.print(" mg/L | Temp: "); Serial.println(systemTemp);
    
    // อัปเดตหน้าจอ GUI
    gui.updateValues(systemTemp, phData.pH, doData.DO);
    gui.updateLoRaStatus(lora.getStatus(), lora.getRSSI(), lora.getLastSentTime());

    // --- 2. เช็คเวลาส่ง LoRa (ทุก 10 วินาที) ---
    if (millis() - lastLoRaTime >= LORA_INTERVAL) {
        lastLoRaTime = millis();
        
        // เตรียมข้อมูล
        float sendTemp = systemTemp;
        float sendPH = (phData.errorCode == 0) ? phData.pH : 0.0;
        float sendDO = (doData.errorCode == 0) ? doData.DO : 0.0;

        // เตรียมสถานะ (True = OK, False = Error)
        // หมายเหตุ: systemTemp เราถือว่า OK ไว้ก่อน หรือจะเช็ค doData.errorCode ก็ได้
        bool statusTemp = true; 
        bool statusPH   = (phData.errorCode == 0);
        bool statusDO   = (doData.errorCode == 0);

        Serial.println(">>> Time to Uplink LoRa <<<");
        
        // ส่งข้อมูลพร้อมสถานะ
        lora.sendSensorData(sendTemp, statusTemp, sendPH, statusPH, sendDO, statusDO);
    }
  }
}