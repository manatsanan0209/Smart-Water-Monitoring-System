#include "SystemGUI.h"
#include <Wire.h>

#ifndef TS_DECLARED
Adafruit_FT6206 ts = Adafruit_FT6206();
#define TS_DECLARED
#endif

SystemGUI::SystemGUI() {}

void SystemGUI::init() {
  Serial.println("[GUI] Init");
  _tft.init();
  _tft.setRotation(3);
  _tft.fillScreen(COLOR_BG);

  Wire.setSDA(4); Wire.setSCL(5);
  Wire.begin();
  if (!ts.begin(40)) Serial.println("TS Error!"); else Serial.println("TS Started");

  // ตั้งค่าเริ่มต้น WiFi เป็นปิด
  WiFi.mode(WIFI_OFF);
  _wifiCount = -1;

  changeScreen(SCREEN_DASHBOARD);
}

void SystemGUI::loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    uint16_t t_x = map(p.y, 0, 320, 0, 320);
    uint16_t t_y = map(p.x, 0, 240, 240, 0);
    handleTouch(t_x, t_y);

    // Serial.print("SENSOR RAW -> p.x: "); Serial.print(p.x); Serial.print(" | p.y: "); Serial.println(p.y);
    // Serial.print("Touch Raw -> X: "); Serial.print(t_x); Serial.print(", Y: "); Serial.println(t_y);
    // _tft.fillCircle(t_x, t_y, 3, TFT_RED);

    delay(50); // ลดจาก 150ms (capacitive touch ไม่ต้องการ debounce นาน)
  }

  if (_currentState == SCREEN_DASHBOARD && _hasData) {
    bool alarm = (_cachePH < settings.phMin || _cachePH > settings.phMax ||
                  _cacheDO < settings.doMin || _cacheDO > settings.doMax);

    if (alarm) {
      if (millis() - _lastBlinkTime > 500) {
        _lastBlinkTime = millis();
        _blinkState = !_blinkState;
        updateDashboardValues();
      }
    } else if (_blinkState) {
      _blinkState = false;
      updateDashboardValues();
    }
  }
}

void SystemGUI::updateValues(float temp, float ph, float DO) {
  _cacheTemp = temp; _cachePH = ph; _cacheDO = DO;
  _hasData = true;
  if (_currentState == SCREEN_DASHBOARD) updateDashboardValues();
}

void SystemGUI::updateLoRaStatus(String status, int rssi, String lastSent) {
  _loraStatus = status;
  _loraRSSI = rssi;
  _loraLastSent = lastSent;
  
  // ถ้าเปิดหน้า LoRa Status อยู่ ให้วาดทับเพื่ออัปเดตข้อมูลทันที
  if (_currentState == SCREEN_LORA_STATUS) {
    drawLoRaStatus();
  }
}

void SystemGUI::changeScreen(int newState) {
  _currentState = newState;
  switch (_currentState) {
    case SCREEN_DASHBOARD:      drawDashboard(); break;
    case SCREEN_RANGE_LIST:     drawRangeList(); break;
    case SCREEN_SETTING:        drawSetting(); break;
    case SCREEN_CALIBRATION:    drawCalibration(); break;
    case SCREEN_EDIT_RANGE:     drawRangeEditScreen(); break;
    case SCREEN_WIFI_SCAN:      drawWifiScan(); break;
    case SCREEN_WIFI_PASSWORD:  drawWifiPasswordScreen(); break;
    case SCREEN_LORA_STATUS:    drawLoRaStatus(); break;
  }
}

void SystemGUI::handleTouch(uint16_t x, uint16_t y) {
  // 1. ปุ่ม Global (Nav & Back)
  if (handleGlobalNavbar(x, y)) return;
  if (handleBackButton(x, y)) return;

  // 2. ปุ่มเฉพาะของแต่ละหน้า
  switch (_currentState) {
    case SCREEN_RANGE_LIST:
      if (isButtonPressed(x, y, 0, 80, 320, 45)) {
        openRangeEdit("#1 PH", &settings.phMin, &settings.phMax, 0.0, 14.0);
      }
      else if (isButtonPressed(x, y, 0, 130, 320, 45)) {
        openRangeEdit("#2 DO", &settings.doMin, &settings.doMax, 0.0, 20.0);
      }
      break;

    case SCREEN_SETTING:
      if (isButtonPressed(x, y, 0, 130, 320, 45)) {
        flashListRow(155, "Calibration");
        changeScreen(SCREEN_CALIBRATION);
      }
      else if (isButtonPressed(x, y, 0, 180, 320, 45)) {
        flashListRow(200, "Show Device QR");
        // Action...
      }
      break;

    case SCREEN_CALIBRATION:
      {
        String items[] = {"#1 PH", "#2 DO", "#3 -"};
        int startY = 110;
        int gap = 45;
        for (int i = 0; i < 3; i++) {
          int yCenter = startY + (i * gap);
          if (isButtonPressed(x, y, 0, yCenter - 25, 320, 45)) {
            flashListRow(yCenter, items[i]);
            Serial.print("Calibrate Clicked: "); Serial.println(items[i]);
            return;
          }
        }
      }
      break;

    case SCREEN_WIFI_SCAN:
      {
        // === กรณี 1: ถ้าเชื่อมต่ออยู่ (Connected Mode) ===
        if (WiFi.status() == WL_CONNECTED) {
           // เช็คปุ่ม Disconnect (X=60, Y=175, W=200, H=40)
           if (isButtonPressed(x, y, 60, 175, 200, 40)) {
               drawButtonEffect(60, 175, 200, 40);
               
               Serial.println("[WiFi] Disconnecting...");
               
               // สั่งตัดการเชื่อมต่อ
               WiFi.disconnect();
               delay(500); // รอสักครู่ให้ Hardware เคลียร์สถานะ
               
               // กลับไปโหมดสแกน
               _wifiCount = 0; // รีเซ็ตค่า
               drawWifiScan();
           }
           return; // จบการทำงานของหน้า Connected
        }

        // === กรณี 2: โหมดสแกนรายการ (Scanning Mode) ===
        // (ส่วนนี้คือโค้ดเดิมทั้งหมดสำหรับ ปุ่ม Toggle, Refresh, Scroll, List)

        // 1. ปุ่ม Toggle ON/OFF
        if (isButtonPressed(x, y, 0, 45, 90, 50)) {
          drawButtonEffect(10, 55, 70, 30);
          bool isOn = (_wifiCount != -1);
          if (isOn) {
            WiFi.disconnect(); WiFi.mode(WIFI_OFF); _wifiCount = -1; drawWifiList();
          } else {
            WiFi.mode(WIFI_STA); delay(500); _wifiCount = 0; drawWifiScan();
          }
          return;
        }

        // 2. ปุ่ม Refresh
        if (WiFi.getMode() == WIFI_STA && isButtonPressed(x, y, 190, 45, 120, 50)) {
          drawButtonEffect(200, 55, 100, 30); drawWifiScan(); return;
        }

        // 3. ปุ่ม Scroll
        if (x > 260) {
          if (y >= 100 && y <= 160) {
            if (_wifiScrollOffset > 0) {
              _wifiScrollOffset--; drawButtonEffect(270, 105, 40, 50); drawWifiList();
            }
          }
          else if (y >= 175 && y <= 235) {
            if (_wifiScrollOffset + 3 < _wifiCount) {
              _wifiScrollOffset++; drawButtonEffect(270, 180, 40, 50); drawWifiList();
            }
          }
        }

        // 4. เลือกรายการ WiFi
        int startY = 105; int gap = 42;
        for (int i = 0; i < 3; i++) {
          int yItem = startY + (i * gap);
          if (isButtonPressed(x, y, 10, yItem, 250, 35)) {
            int listIndex = _wifiScrollOffset + i;
            if (listIndex < _wifiCount) {
              if (_wifiSortedIndices != nullptr) {
                _selectedSSID = WiFi.SSID(_wifiSortedIndices[listIndex]);
              } else {
                _selectedSSID = WiFi.SSID(listIndex);
              }
              _enteredPassword = ""; 
              drawButtonEffect(10, yItem, 250, 35);
              changeScreen(SCREEN_WIFI_PASSWORD);
            }
            return;
          }
        }
      }
      break;

    case SCREEN_WIFI_PASSWORD:
      handleWifiPasswordTouch(x, y);
      break;

    case SCREEN_EDIT_RANGE:
      handleRangeEditTouch(x, y);
      break;
  }
}