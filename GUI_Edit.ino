#include "SystemGUI.h"

void SystemGUI::openRangeEdit(String title, float* minPtr, float* maxPtr, float absMin, float absMax) {
  _editTitle = title;
  _ptrMin = minPtr;      
  _ptrMax = maxPtr;      
  _tempMin = *minPtr;    
  _tempMax = *maxPtr;
  _limitAbsMin = absMin; 
  _limitAbsMax = absMax; 
  changeScreen(SCREEN_EDIT_RANGE);
}

void SystemGUI::drawRangeEditScreen() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(true); 
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);

  // Position Config
  int yBox = 85;       
  int yTitle = 110;    
  int arrUpTip = 55;   
  int arrUpBase = 75;  
  int arrDownBase = 125; 
  int arrDownTip = 145;  

  // 1. Title
  _tft.setCursor(10, yTitle); _tft.print(_editTitle);

  int xL = 140; int xR = 250; 
  int boxW = 80; int halfW = 40;

  // 2. Min Column
  _tft.fillTriangle(xL, arrUpTip, xL-15, arrUpBase, xL+15, arrUpBase, COLOR_TEXT);
  _tft.drawRect(xL-halfW, yBox, boxW, 35, COLOR_TEXT);                             
  _tft.setCursor(xL-28, yBox+25); _tft.print(_tempMin, 1);                         
  _tft.fillTriangle(xL, arrDownTip, xL-15, arrDownBase, xL+15, arrDownBase, COLOR_TEXT);

  _tft.setCursor(190, yTitle); _tft.print("-");

  // 3. Max Column
  _tft.fillTriangle(xR, arrUpTip, xR-15, arrUpBase, xR+15, arrUpBase, COLOR_TEXT);
  _tft.drawRect(xR-halfW, yBox, boxW, 35, COLOR_TEXT);                             
  _tft.setCursor(xR-28, yBox+25); _tft.print(_tempMax, 1);                         
  _tft.fillTriangle(xR, arrDownTip, xR-15, arrDownBase, xR+15, arrDownBase, COLOR_TEXT);

  // 4. Buttons (OK / Cancel) - ปรับให้เป็นสีขาวและตัวหนา
  int btnY = 190; int btnW = 100; int btnH = 40;

  // --- OK Box (Checkmark) ---
  _tft.drawRect(50, btnY, btnW, btnH, COLOR_TEXT); 
  int okX = 100; int okY = btnY + 20;
  
  // วาดเครื่องหมายถูก (สีขาว + หนา 3px)
  for(int i=0; i<3; i++) {
     _tft.drawLine(okX-10, okY+i, okX, okY+10+i, TFT_WHITE); 
     _tft.drawLine(okX, okY+10+i, okX+20, okY-15+i, TFT_WHITE);
  }
  
  // --- Cancel Box (X) ---
  _tft.drawRect(170, btnY, btnW, btnH, COLOR_TEXT); 
  int crossX = 220; int crossY = btnY + 20;
  int sz = 8; // ขนาดกากบาท

  // วาดกากบาท (สีขาว + หนา 3px) โดยการวาดเส้นขนาน
  for(int i=-1; i<=1; i++) {
    _tft.drawLine(crossX-sz+i, crossY-sz, crossX+sz+i, crossY+sz, TFT_WHITE); // เส้นเฉียงลง
    _tft.drawLine(crossX+sz+i, crossY-sz, crossX-sz+i, crossY+sz, TFT_WHITE); // เส้นเฉียงขึ้น
  }
}

void SystemGUI::handleRangeEditTouch(uint16_t x, uint16_t y) {
  bool needUpdate = false;
  int xL = 140; int xR = 250;

  // Arrow Zones
  if (isButtonPressed(x, y, xL-30, 45, 60, 40)) { _tempMin += 0.1; if(_tempMin >= _tempMax) _tempMin = _tempMax-0.1; needUpdate=true; }
  else if (isButtonPressed(x, y, xL-30, 115, 60, 40)) { _tempMin -= 0.1; if(_tempMin < _limitAbsMin) _tempMin = _limitAbsMin; needUpdate=true; }
  else if (isButtonPressed(x, y, xR-30, 45, 60, 40)) { _tempMax += 0.1; if(_tempMax > _limitAbsMax) _tempMax = _limitAbsMax; needUpdate=true; }
  else if (isButtonPressed(x, y, xR-30, 115, 60, 40)) { _tempMax -= 0.1; if(_tempMax <= _tempMin) _tempMax = _tempMin+0.1; needUpdate=true; }

  // Action Buttons (ขยายพื้นที่กดให้ง่ายขึ้น)
  int btnY = 190;
  if (isButtonPressed(x, y, 45, btnY-5, 110, 50)) { // OK
    drawButtonEffect(50, btnY, 100, 40); 
    *_ptrMin = _tempMin; *_ptrMax = _tempMax;
    changeScreen(SCREEN_RANGE_LIST);
    return;
  }
  if (isButtonPressed(x, y, 165, btnY-5, 110, 50)) { // Cancel
    drawButtonEffect(170, btnY, 100, 40); 
    changeScreen(SCREEN_RANGE_LIST);
    return;
  }

  if (needUpdate) {
    _tft.setFreeFont(FMB12); _tft.setTextColor(COLOR_TEXT, COLOR_BG);
    _tft.fillRect(102, 87, 76, 31, COLOR_BG);
    _tft.setCursor(xL-28, 110); _tft.print(_tempMin, 1);
    _tft.fillRect(212, 87, 76, 31, COLOR_BG);
    _tft.setCursor(xR-28, 110); _tft.print(_tempMax, 1);
  }
}