#include "SystemGUI.h"

void SystemGUI::drawHeader(bool showBackBtn) {
  int iconY = 10;
  
  // 1. วาดไอคอนเดิม (ระฆัง, เฟือง)
  _tft.drawBitmap(ICON_BELL_X, iconY, icon_bell_bits, icon_width, icon_height, TFT_WHITE);
  _tft.drawBitmap(ICON_GEAR_X, iconY, icon_gear_bits, icon_width, icon_height, TFT_WHITE); 

  // 2. วาดไอคอน LoRa (ที่เพิ่มเข้าไปใหม่)
  uint16_t loraColor = (_loraStatus == "Joined") ? TFT_GREEN : TFT_DARKGREY;
  _tft.drawBitmap(ICON_LORA_X, iconY, icon_lora_bits, icon_width, icon_height, loraColor);
  
  // 3. วาดไอคอน WiFi
  uint16_t wifiColor;
  bool isWifiOff = false;
  if (WiFi.status() == WL_CONNECTED) wifiColor = TFT_GREEN;
  else if (WiFi.getMode() == WIFI_STA) wifiColor = TFT_WHITE;
  else { wifiColor = TFT_DARKGREY; isWifiOff = true; }
  
  _tft.drawBitmap(ICON_WIFI_X, iconY, icon_wifi_bits, icon_width, icon_height, wifiColor);
  if (isWifiOff) {
     int x = ICON_WIFI_X; int y = iconY; int w = icon_width; int h = icon_height;
     _tft.drawLine(x + 12, y + 12, x + w - 4, y + h - 4, TFT_RED);
     _tft.drawLine(x + w - 4, y + 12, x + 12, y + h - 4, TFT_RED);
  }

  // 4. ส่วน Logo และปุ่ม Back (จุดที่ต้องแก้!)
  if (showBackBtn) {
    // --- แก้ตรงนี้ครับ จาก 180 เป็น 140 ---
    // เพื่อไม่ให้มันยาวไปทับไอคอน LoRa (ที่อยู่ตำแหน่ง 155)
    _tft.fillRect(0, 0, 140, 40, COLOR_BG); 
    // ------------------------------------
    
    _tft.setFreeFont(FMB12);
    _tft.setTextColor(TFT_WHITE, COLOR_BG);
    _tft.setCursor(10, 25); _tft.print("<");
    _tft.drawBitmap(40, 8, icon_logo_bits, LOGO_WIDTH, LOGO_HEIGHT, TFT_GREEN);
  } else {
    _tft.drawBitmap(10, 8, icon_logo_bits, LOGO_WIDTH, LOGO_HEIGHT, TFT_GREEN);
  }
}

// Effect แถบเมนู (พื้นขาว ตัวหนังสือดำ)
void SystemGUI::flashListRow(int textY, String text) {
  int rectY = textY - 25; 
  int rectH = 40;         
  
  _tft.fillRect(0, rectY, 320, rectH, TFT_WHITE); 
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(TFT_BLACK, TFT_WHITE); 
  _tft.setCursor(10, textY); _tft.print(text);
  _tft.setCursor(290, textY); _tft.print(">");
  
  delay(100); 
  
  _tft.fillRect(0, rectY, 320, rectH, COLOR_BG);
  _tft.setTextColor(TFT_WHITE, COLOR_BG);
  _tft.setCursor(10, textY); _tft.print(text);
  _tft.setCursor(290, textY); _tft.print(">");
  _tft.drawLine(10, textY + 15, 310, textY + 15, TFT_WHITE);
}

// Effect ปุ่มทั่วไป (ขอบมนสีขาว)
void SystemGUI::drawButtonEffect(int x, int y, int w, int h) {
  int radius = 6;
  _tft.drawRoundRect(x, y, w, h, radius, TFT_WHITE); 
  delay(100); 
  _tft.drawRoundRect(x, y, w, h, radius, COLOR_BG);
}

// Global Navbar Logic
bool SystemGUI::handleGlobalNavbar(uint16_t x, uint16_t y) {
  int effectSize = 34; 
  int offset = 5; 
  int iconY = 10;

  // 1. ปุ่ม Bell
  if (isButtonPressed(x, y, ICON_BELL_X, 0, 45, NAV_H)) {
    if (_currentState != SCREEN_RANGE_LIST) {
      drawButtonEffect(ICON_BELL_X - offset, iconY - offset, effectSize, effectSize);
      changeScreen(SCREEN_RANGE_LIST);
    }
    return true;
  }

  // 2. ปุ่ม Gear
  if (isButtonPressed(x, y, ICON_GEAR_X, 0, 45, NAV_H)) {
    if (_currentState != SCREEN_SETTING) {
      drawButtonEffect(ICON_GEAR_X - offset, iconY - offset, effectSize, effectSize);
      changeScreen(SCREEN_SETTING);
    }
    return true;
  }

  // 3. ปุ่ม WiFi (เพิ่มใหม่)
  if (isButtonPressed(x, y, ICON_WIFI_X, 0, 45, NAV_H)) {
     drawButtonEffect(ICON_WIFI_X - offset, iconY - offset, effectSize, effectSize);
     changeScreen(SCREEN_WIFI_SCAN); 
     
     return true;
  }

  if (isButtonPressed(x, y, ICON_LORA_X, 0, 45, NAV_H)) {
     drawButtonEffect(ICON_LORA_X - offset, iconY - offset, effectSize, effectSize);
     changeScreen(SCREEN_LORA_STATUS); 
     return true;
  }

  return false;
}

// Global Back Logic
bool SystemGUI::handleBackButton(uint16_t x, uint16_t y) {
  // ถ้าอยู่หน้า Dashboard ไม่ต้องมีปุ่ม Back
  if (_currentState == SCREEN_DASHBOARD) return false;

  // ตรวจสอบพิกัดปุ่ม Back (มุมซ้ายบน)
  if (isButtonPressed(x, y, 0, 0, 85, 40)) {
    drawButtonEffect(5, 2, 80, 36); // Effect ปุ่ม

    // --- Logic การย้อนกลับ ---
    if (_currentState == SCREEN_CALIBRATION) {
       changeScreen(SCREEN_SETTING);
    } 
    else if (_currentState == SCREEN_EDIT_RANGE) {
       changeScreen(SCREEN_RANGE_LIST);
    }
    // เพิ่ม: ถ้าอยู่หน้าใส่รหัส -> กลับไปหน้าสแกน
    else if (_currentState == SCREEN_WIFI_PASSWORD) {
       changeScreen(SCREEN_WIFI_SCAN);
    }
    else if (_currentState == SCREEN_LORA_STATUS) {
       changeScreen(SCREEN_DASHBOARD);
    }
    // อื่นๆ -> กลับไปหน้าหลัก (Dashboard)
    else {
       changeScreen(SCREEN_DASHBOARD);
    }
    return true;
  }
  return false;
}

bool SystemGUI::isButtonPressed(uint16_t x, uint16_t y, int bx, int by, int bw, int bh) {
  return (x >= bx && x <= (bx + bw) && y >= by && y <= (by + bh));
}

void SystemGUI::showError(String msg) { /* Error logic */ }