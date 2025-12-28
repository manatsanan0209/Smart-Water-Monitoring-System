#include "PHSensorReader.h"
#include "SystemGUI.h"

// ================= CONFIGURATION =================
// Pin Definitions
const int PIN_RS485_DIR = 2;
const int PIN_TFT_CS    = 17;
const int PIN_TFT_DC    = 20;
const int PIN_TFT_RST   = 21;
const int PIN_RESET_BTN = 15;

// Timing
const int READ_INTERVAL = 2000; // อ่านค่าทุก 2 วินาที

// ================= OBJECTS =================
PHSensorReader sensor(PIN_RS485_DIR);
SystemGUI gui(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

void setup() {
  Serial.begin(115200);
  
  // Setup ปุ่มกู้ชีพ
  pinMode(PIN_RESET_BTN, INPUT_PULLUP);

  // เริ่มต้นระบบย่อย
  sensor.begin();
  gui.init();

  Serial.println("=== System Started ===");
}

void loop() {
  // ------------------------------------------------
  // 1. ตรวจสอบปุ่ม Manual Reset (ระบบกู้ชีพจอ)
  // ------------------------------------------------
  if (digitalRead(PIN_RESET_BTN) == LOW) {
    Serial.println("!!! MANUAL RESET TRIGGERED !!!");
    gui.init();      // เรียกใช้ฟังก์ชัน init เดิมเพื่อรีเซ็ตจอ
    delay(500);      // Debounce ปุ่ม
    return;
  }

  // ------------------------------------------------
  // 2. อ่านค่าจาก Sensor
  // ------------------------------------------------
  Serial.println("Reading Sensor...");
  SensorResult data = sensor.read();

  // ------------------------------------------------
  // 3. แสดงผล (แยกกรณีสำเร็จ / ไม่สำเร็จ)
  // ------------------------------------------------
  if (data.errorCode == 0) {
    // อ่านสำเร็จ -> Log ลง Serial และขึ้นจอ
    Serial.print("pH: "); Serial.print(data.pH);
    Serial.print(" | Temp: "); Serial.println(data.temp);
    
    gui.updateValues(data.temp, data.pH);
    
  } else {
    // อ่านพลาด -> แจ้ง Error
    Serial.print("Read Failed: ");
    Serial.println(data.errorMsg);
    
    gui.showError(data.errorMsg);
  }

  delay(READ_INTERVAL);
}