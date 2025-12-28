#include "SystemGUI.h"

SystemGUI::SystemGUI(int8_t cs, int8_t dc, int8_t rst) 
  : _tft(cs, dc, rst) { // Initialize list
}

void SystemGUI::init() {
  Serial.println("[GUI] Initializing Display...");
  _tft.begin();
  _tft.setRotation(3);
  _tft.fillScreen(ILI9341_BLACK);
  
  drawInterface();
  Serial.println("[GUI] Display Ready");
}

void SystemGUI::drawInterface() {
  // หัวข้อ
  _tft.setTextColor(ILI9341_WHITE); 
  _tft.setTextSize(2);
  _tft.setCursor(60, 10);
  _tft.println("pH Monitoring");
  
  _tft.drawFastHLine(0, 35, 320, ILI9341_WHITE);

  // Label: pH
  _tft.setTextColor(ILI9341_CYAN);
  _tft.setTextSize(3);
  _tft.setCursor(20, 60);
  _tft.print("pH Value:");

  // Label: Temp
  _tft.setTextColor(ILI9341_YELLOW);
  _tft.setTextSize(3);
  _tft.setCursor(20, 140);
  _tft.print("Temp (C):");
}

void SystemGUI::updateValues(float temp, float ph) {
  // 1. แสดงค่า pH
  _tft.setTextSize(4);
  _tft.setCursor(180, 55);
  
  // Logic เลือกสี
  if (ph < 6.0) _tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  else if (ph > 8.0) _tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
  else _tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);

  _tft.print(ph, 2);

  // 2. แสดงค่า Temperature
  _tft.setTextSize(4);
  _tft.setCursor(180, 135);
  _tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  _tft.print(temp, 1);
  
  // ลบข้อความ Error เก่า (ถ้ามี) โดยการเขียนทับด้วยสีดำ
  _tft.setCursor(10, 200);
  _tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK); 
  _tft.setTextSize(2);
  _tft.print("Error: Response Timed Out"); // ลบข้อความที่ยาวที่สุดที่อาจเกิดขึ้น
}

void SystemGUI::showError(String msg) {
  _tft.setCursor(10, 200);
  _tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  _tft.setTextSize(2);
  _tft.print("Error: ");
  _tft.print(msg);
}