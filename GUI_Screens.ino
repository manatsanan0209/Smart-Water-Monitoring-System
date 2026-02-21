#include "SystemGUI.h"

// --- Dashboard ---
void SystemGUI::drawDashboard() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(false); 
  _tft.setFreeFont(FMB12); 
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);

  // บรรทัดที่ 1: #1 PH (ตำแหน่งเดิม Y=80)
  _tft.setCursor(35, 80);   _tft.print("#1 PH");
  _tft.setCursor(135, 80);  _tft.print(":");

  // บรรทัดที่ 2: #2 DO (ย้ายขึ้นมาจาก Y=160 เป็น Y=120)
  _tft.setCursor(35, 120);  _tft.print("#2 DO");
  _tft.setCursor(135, 120); _tft.print(":");
  
  // บรรทัดที่ 3: TEMP (ย้ายลงมาจาก Y=120 เป็น Y=160 และอยู่ใต้ DO)
  _tft.setCursor(65, 160);  _tft.print("TEMP");   // ย่อหน้าเข้าไป (X=65) เพื่อให้รู้ว่าเป็นลูกย่อย
  _tft.setCursor(135, 160); _tft.print(":");     
  
  // บรรทัดที่ 4: #3 (ตำแหน่งเดิม Y=200)
  _tft.setCursor(35, 200);  _tft.print("#3 -");
  
  if (_hasData) updateDashboardValues();
}

void SystemGUI::updateDashboardValues() {
  _tft.setFreeFont(FMB12);

  // --- 1. Update #1 PH (Y=80) ---
  bool isPhAlarm = (_cachePH < settings.phMin || _cachePH > settings.phMax);
  uint16_t phBg = (isPhAlarm && _blinkState) ? COLOR_ALERT : COLOR_BG;
  uint16_t phTxt = (isPhAlarm && !_blinkState) ? COLOR_ALERT : COLOR_TEXT;
  
  _tft.fillRect(0, 50, 320, 45, phBg); // พื้นหลังแถวที่ 1
  _tft.setTextColor(phTxt, phBg);
  
  _tft.setCursor(35, 80);  _tft.print("#1 PH");
  _tft.setCursor(135, 80); _tft.print(":");
  _tft.setCursor(155, 80); _tft.print(_cachePH, 1);

  if (isPhAlarm) {
    _tft.drawBitmap(ALARM_ICON_X, 62, icon_warning_bits, WARNING_ICON_W, WARNING_ICON_H, _blinkState ? COLOR_TEXT : COLOR_ALERT);
  }

  // --- 2. Update #2 DO (ย้ายมาตำแหน่ง Y=120) ---
  bool isDoAlarm = (_cacheDO < settings.doMin || _cacheDO > settings.doMax);
  uint16_t doBg = (isDoAlarm && _blinkState) ? COLOR_ALERT : COLOR_BG;
  uint16_t doTxt = (isDoAlarm && !_blinkState) ? COLOR_ALERT : COLOR_TEXT;

  // ปรับพิกัด FillRect ให้ตรงกับแถวที่ 2 (เริ่ม Y=95 ถึง 140)
  _tft.fillRect(0, 95, 320, 45, doBg); 
  _tft.setTextColor(doTxt, doBg);
  
  _tft.setCursor(35, 120);  _tft.print("#2 DO");
  _tft.setCursor(135, 120); _tft.print(":");
  _tft.setCursor(155, 120); _tft.print(_cacheDO, 1);
  _tft.print(" mg/L");

  if (isDoAlarm) {
    // ปรับตำแหน่งไอคอนเตือนให้ตรงกับแถวใหม่ (Y=107)
    _tft.drawBitmap(ALARM_ICON_X, 107, icon_warning_bits, WARNING_ICON_W, WARNING_ICON_H, _blinkState ? COLOR_TEXT : COLOR_ALERT);
  }

  // --- 3. Update TEMP (ย้ายมาตำแหน่ง Y=160) ---
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  // ล้างค่าเก่าที่ตำแหน่งใหม่ (Y=160)
  _tft.fillRect(155, 140, 150, 30, COLOR_BG); 
  _tft.setCursor(155, 160); 
  _tft.print((int)_cacheTemp);
  _tft.print(" C");
}

// --- Range List ---
void SystemGUI::drawRangeList() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(true);
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  
  _tft.setCursor(10, 70); _tft.print("Range Setting");
  _tft.drawLine(10, 75, 200, 75, COLOR_TEXT);

  // Draw List Items
  int y = 110;
  _tft.setCursor(10, y); _tft.print("#1 PH");
  _tft.drawRect(100, y-25, 80, 35, COLOR_TEXT);
  _tft.setCursor(110, y); _tft.print(settings.phMin, 1);
  _tft.setCursor(190, y); _tft.print("-");
  _tft.drawRect(210, y-25, 80, 35, COLOR_TEXT);
  _tft.setCursor(220, y); _tft.print(settings.phMax, 1);

  y = 160;
  _tft.setCursor(10, y); _tft.print("#2 DO");
  _tft.drawRect(100, y-25, 80, 35, COLOR_TEXT);
  _tft.setCursor(110, y); _tft.print(settings.doMin, 1);
  _tft.setCursor(190, y); _tft.print("-");
  _tft.drawRect(210, y-25, 80, 35, COLOR_TEXT);
  _tft.setCursor(220, y); _tft.print(settings.doMax, 1);
}

// --- Settings ---
void SystemGUI::drawSetting() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(true);
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  
  _tft.setCursor(10, 70); _tft.print("Setting");
  _tft.drawLine(10, 75, 100, 75, COLOR_TEXT);

  int y = 110;
  _tft.setCursor(10, y); _tft.print("Salinity Lv");
  _tft.drawRect(230, y-25, 70, 35, COLOR_TEXT);
  _tft.setCursor(245, y); _tft.print("8.5");
  _tft.drawLine(10, y+15, 310, y+15, COLOR_TEXT);

  y = 155;
  _tft.setCursor(10, y); _tft.print("Calibration");
  _tft.setCursor(290, y); _tft.print(">"); 
  _tft.drawLine(10, y+15, 310, y+15, COLOR_TEXT);

  y = 200;
  _tft.setCursor(10, y); _tft.print("Show Device QR");
  _tft.setCursor(290, y); _tft.print(">"); 
}

// --- Calibration (Loop Reuse) ---
void SystemGUI::drawCalibration() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(true);
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  
  _tft.setCursor(10, 70); _tft.print("Calibration");
  _tft.drawLine(10, 75, 160, 75, COLOR_TEXT);

  String items[] = {"#1 PH", "#2 DO", "#3 -"};
  int startY = 110;
  int gap = 45;

  for(int i=0; i<3; i++) {
    int y = startY + (i * gap);
    _tft.setCursor(10, y); _tft.print(items[i]);
    _tft.setCursor(290, y); _tft.print(">");
    _tft.drawLine(10, y+15, 310, y+15, COLOR_TEXT);
  }
}

// --- ฟังก์ชันสแกน (แก้ไขใหม่: เช็ค Connected ก่อน) ---
void SystemGUI::drawWifiScan() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(true);

  // 1. ถ้าเชื่อมต่ออยู่แล้ว (WL_CONNECTED)
  if (WiFi.status() == WL_CONNECTED) {
     // ไม่ต้องสแกน ให้ไปวาดหน้าสถานะเชื่อมต่อเลย
     _wifiCount = 1; // ตั้งค่าสมมติว่าเปิดอยู่
     drawWifiList(); 
     return;
  }

  // 2. ถ้ายังไม่เชื่อมต่อ -> เช็คว่าปิดอยู่ไหม
  if (WiFi.getMode() != WIFI_STA && _wifiCount == -1) {
    _wifiCount = -1;
    drawWifiList();
    return;
  }

  // 3. เริ่มสแกน
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  _tft.setCursor(10, 80); _tft.print("Scanning...");
  
  int n = WiFi.scanNetworks();
  
  if (n >= 0) {
    _wifiCount = n;
    
    // Sort Logic (เหมือนเดิม)
    if (_wifiSortedIndices != nullptr) delete[] _wifiSortedIndices;
    _wifiSortedIndices = new int[_wifiCount];
    for (int i = 0; i < _wifiCount; i++) _wifiSortedIndices[i] = i;
    for (int i = 0; i < _wifiCount - 1; i++) {
      for (int j = 0; j < _wifiCount - i - 1; j++) {
        if (WiFi.RSSI(_wifiSortedIndices[j]) < WiFi.RSSI(_wifiSortedIndices[j + 1])) {
          int temp = _wifiSortedIndices[j];
          _wifiSortedIndices[j] = _wifiSortedIndices[j + 1];
          _wifiSortedIndices[j + 1] = temp;
        }
      }
    }
  } else {
    _wifiCount = -1; 
  }

  drawWifiList();
}

// --- ฟังก์ชันวาดหน้าจอ (แก้ไขใหม่: เพิ่มหน้า Connected UI) ---
void SystemGUI::drawWifiList() {
  _tft.fillRect(0, 50, 320, 190, COLOR_BG);

  // === กรณี 1: เชื่อมต่อ WiFi อยู่ ===
  if (WiFi.status() == WL_CONNECTED) {
      _tft.setFreeFont(FMB12);
      _tft.setTextColor(TFT_GREEN, COLOR_BG);
      _tft.setCursor(20, 80); _tft.print("Status: Connected");
      
      // ชื่อ WiFi
      _tft.setTextColor(TFT_WHITE, COLOR_BG);
      _tft.setCursor(20, 115); 
      _tft.print("SSID: ");
      String ssid = WiFi.SSID();
      if(ssid.length() > 10) ssid = ssid.substring(0, 10) + "..";
      _tft.print(ssid);

      // IP Address
      _tft.setFreeFont(FMB9);
      _tft.setTextColor(TFT_LIGHTGREY, COLOR_BG);
      _tft.setCursor(20, 145); 
      _tft.print("IP: "); _tft.print(WiFi.localIP());

      // ปุ่ม Disconnect (ปุ่มแดงใหญ่)
      int btnX = 60; int btnY = 175; int btnW = 200; int btnH = 40;
      _tft.fillRoundRect(btnX, btnY, btnW, btnH, 5, TFT_RED);
      _tft.setFreeFont(FMB12);
      _tft.setTextColor(TFT_WHITE);
      _tft.setCursor(btnX + 35, btnY + 28); _tft.print("Disconnect");
      
      return; // จบการทำงาน (ไม่วาด List ต่อ)
  }

  // === กรณี 2: ยังไม่เชื่อมต่อ (วาด List แบบเดิม) ===
  
  // ส่วนควบคุม (Control Bar)
  int btnY = 55; int btnH = 30;
  _tft.setFreeFont(FMB9);

  bool isOn = (_wifiCount != -1); 

  // ปุ่ม Toggle
  if (isOn) {
    _tft.fillRoundRect(10, btnY, 70, btnH, 5, TFT_GREEN);
    _tft.setTextColor(TFT_BLACK);
    _tft.setCursor(20, btnY + 20); _tft.print("ON");
  } else {
    _tft.fillRoundRect(10, btnY, 70, btnH, 5, TFT_RED);
    _tft.setTextColor(TFT_WHITE);
    _tft.setCursor(15, btnY + 20); _tft.print("OFF");
  }
  
  // ปุ่ม Refresh
  if (isOn) {
    _tft.drawRoundRect(200, btnY, 100, btnH, 5, TFT_WHITE);
    _tft.setTextColor(TFT_WHITE, COLOR_BG);
    _tft.setCursor(215, btnY + 20); _tft.print("Refresh");
  }

  _tft.drawLine(10, 92, 310, 92, TFT_DARKGREY);

  if (!isOn) {
    _tft.setFreeFont(FMB12); _tft.setTextColor(TFT_LIGHTGREY, COLOR_BG);
    _tft.setCursor(60, 160); _tft.print("WiFi is OFF");
    return;
  }

  if (_wifiCount == 0) {
    _tft.setFreeFont(FMB12); _tft.setTextColor(COLOR_TEXT, COLOR_BG);
    _tft.setCursor(10, 130); _tft.print("No Networks");
    return;
  }

  // วาดรายการ
  int startY = 105; int gap = 42; int limit = 3; 
  for (int i = 0; i < limit; ++i) {
    int listIndex = _wifiScrollOffset + i;
    if (listIndex >= _wifiCount) break;

    int originalIndex = (_wifiSortedIndices != nullptr) ? _wifiSortedIndices[listIndex] : listIndex;

    int y = startY + (i * gap);
    _tft.drawRoundRect(10, y, 250, 35, 5, TFT_WHITE);
    
    String ssid = WiFi.SSID(originalIndex);
    if(ssid.length() > 10) ssid = ssid.substring(0, 10) + "..";
    
    _tft.setFreeFont(FMB12);
    _tft.setTextColor(TFT_WHITE, COLOR_BG);
    _tft.setCursor(20, y + 24);
    _tft.print(listIndex + 1); _tft.print(". "); _tft.print(ssid);

    int rssi = WiFi.RSSI(originalIndex);
    int bars = (rssi > -60) ? 3 : (rssi > -80 ? 2 : 1);
    
    int barW = 4; int barGap = 2; int barX = 235; int barBottom = y+28;
    for (int b = 0; b < 3; b++) {
      int h = (b + 1) * 5; 
      if (b < bars) _tft.fillRect(barX + (b * (barW+barGap)), barBottom - h, barW, h, TFT_GREEN);
      else _tft.drawRect(barX + (b * (barW+barGap)), barBottom - h, barW, h, TFT_DARKGREY);
    }
  }

  // ปุ่ม Scroll
  int scrollBtnX = 270; int scrollBtnW = 40; int scrollBtnH = 50; 
  if (_wifiScrollOffset > 0) {
    _tft.drawRect(scrollBtnX, 105, scrollBtnW, scrollBtnH, TFT_WHITE);
    _tft.fillTriangle(scrollBtnX+20, 115, scrollBtnX+5, 145, scrollBtnX+35, 145, TFT_WHITE);
  }
  if (_wifiScrollOffset + limit < _wifiCount) {
    _tft.drawRect(scrollBtnX, 180, scrollBtnW, scrollBtnH, TFT_WHITE);
    _tft.fillTriangle(scrollBtnX+20, 220, scrollBtnX+5, 190, scrollBtnX+35, 190, TFT_WHITE);
  }
}

// --- เพิ่มฟังก์ชันใหม่: วาดหน้ากรอกรหัส ---
void SystemGUI::drawWifiPasswordScreen() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(true); 

  // ปรับตำแหน่ง Y ของส่วนต่างๆ ให้ขยับขึ้นสูงขึ้น
  int topY = 55; // ขยับข้อความขึ้นมาเกือบชิด Header

  _tft.setFreeFont(FMB9);
  _tft.setTextColor(TFT_LIGHTGREY, COLOR_BG);
  _tft.setCursor(10, topY); _tft.print("Pass for: ");
  
  String dispSSID = _selectedSSID;
  if(dispSSID.length() > 10) dispSSID = dispSSID.substring(0, 10) + "..";
  _tft.setTextColor(TFT_GREEN, COLOR_BG);
  _tft.print(dispSSID);

  // ช่องกรอกรหัส (ขยับขึ้นมาที่ Y = 65)
  _tft.drawRect(10, 65, 230, 30, TFT_WHITE);
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(TFT_WHITE, COLOR_BG);
  _tft.setCursor(15, 86); 
  _tft.print(_enteredPassword);

  // ปุ่ม Connect (ขยับขึ้นมาที่ Y = 65)
  _tft.fillRoundRect(250, 65, 65, 30, 5, TFT_GREEN);
  _tft.setFreeFont(FMB9);
  _tft.setTextColor(TFT_BLACK);
  _tft.setCursor(253, 85); _tft.print("Conn");

  // วาดคีย์บอร์ด
  drawKeyboard();
}

void SystemGUI::drawKeyboard() {
  _tft.setFreeFont(FMB9);
  _tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // ขยับจุดเริ่มต้นคีย์บอร์ดขึ้นมาที่ Y = 100 (จากเดิม 115/105)
  int startY = 100; 
  int keyW = 30;
  int keyH = 26; // ลดความสูงปุ่มลงเล็กน้อยเพื่อประหยัดพื้นที่
  int gap = 2;

  String rows[] = {
    "1234567890",
    _isShift ? "QWERTYUIOP" : "qwertyuiop",
    _isShift ? "ASDFGHJKL" : "asdfghjkl",
    _isShift ? "ZXCVBNM" : "zxcvbnm"
  };
  int offsets[] = {2, 2, 15, 50}; 
  int lens[] = {10, 10, 9, 7};

  for (int r = 0; r < 4; r++) {
    for (int i = 0; i < lens[r]; i++) {
      int x = offsets[r] + i * (keyW + gap);
      int y = startY + r * (keyH + gap);
      _tft.drawRect(x, y, keyW, keyH, TFT_DARKGREY);
      _tft.setCursor(x + 8, y + 19);
      _tft.print(rows[r][i]);
    }
  }

  // --- ปุ่ม Shift (ใช้ลูกศร ^) ---
  int shiftX = 2;
  int shiftY = startY + 3 * (keyH + gap);
  int shiftW = 45;
  _tft.fillRoundRect(shiftX, shiftY, shiftW, keyH, 3, _isShift ? TFT_BLUE : TFT_DARKGREY);
  _tft.setCursor(shiftX + 15, shiftY + 19);
  _tft.print("^"); 

  // --- ปุ่ม Delete (DEL) ---
  int delX = 265; 
  int delY = startY + 3 * (keyH + gap);
  _tft.fillRoundRect(delX, delY, 52, keyH, 3, TFT_RED);
  _tft.setCursor(delX + 8, delY + 19);
  _tft.print("DEL");

  // --- ปุ่ม Spacebar (ขยับขึ้นมาเยอะพอที่จะไม่ตกขอบแน่นอน) ---
  int spaceX = 80;
  int spaceY = startY + 4 * (keyH + gap);
  _tft.drawRoundRect(spaceX, spaceY, 160, keyH, 3, TFT_WHITE);
  _tft.setCursor(spaceX + 55, spaceY + 19);
  _tft.print("SPACE");
}

// ฟังก์ชันจัดการ Touch คีย์บอร์ดและปุ่ม Connect (แก้ไข Logic การเชื่อมต่อ)
void SystemGUI::handleWifiPasswordTouch(uint16_t x, uint16_t y) {
  
  // 1. ปุ่ม Connect (พิกัด Y=65)
  if (isButtonPressed(x, y, 250, 65, 65, 30)) {
     drawButtonEffect(250, 65, 65, 30);

     // --- แสดงสถานะ Connecting ---
     // ล้างเฉพาะด้านในกล่อง (ไม่ทับเส้นขอบ)
     _tft.fillRect(11, 66, 228, 28, COLOR_BG); 
     
     // พิมพ์ข้อความ Connecting (ใช้ฟอนต์เดิม พิกัด Y=86)
     _tft.setFreeFont(FMB12); 
     _tft.setTextColor(TFT_BLUE, COLOR_BG); 
     _tft.setCursor(15, 86); 
     _tft.print("Connecting.."); // ตัดให้สั้นลงนิดนึงกันตกขอบ

     // --- เตรียมการเชื่อมต่อ ---
     WiFi.disconnect();
     delay(100);
     WiFi.mode(WIFI_STA);
     delay(100);
     
     WiFi.begin(_selectedSSID.c_str(), _enteredPassword.c_str());
     
     unsigned long start = millis();
     bool connected = false;
     int dotCount = 0;

     // รอผลเชื่อมต่อ
     while(millis() - start < 15000) {
        if (WiFi.status() == WL_CONNECTED) {
           connected = true;
           break;
        }
        // ลูกเล่นจุดไข่ปลาวิ่ง ...
        if (millis() % 500 == 0) {
           _tft.print(".");
           dotCount++;
           if(dotCount > 3) { 
             // ล้างแล้วพิมพ์ใหม่ถ้ายาวเกิน
             _tft.fillRect(130, 66, 50, 28, COLOR_BG); 
             _tft.setCursor(145, 86); 
             dotCount = 0; 
           }
        }
        delay(100);
     }

     // --- สรุปผล ---
     if (connected) {
        // เชื่อมต่อติด: ถมเขียวเต็มกล่อง
        _tft.fillRect(10, 65, 230, 30, TFT_GREEN);
        _tft.setCursor(15, 86); 
        _tft.setTextColor(TFT_BLACK); // ตัวหนังสือดำบนพื้นเขียว
        _tft.print("Connected!");
        
        _wifiCount = 1; 
        delay(1500);
        changeScreen(SCREEN_WIFI_SCAN);
     } else {
        // เชื่อมต่อไม่ติด: ถมแดงเต็มกล่อง
        _tft.fillRect(10, 65, 230, 30, TFT_RED);
        _tft.setCursor(15, 86); 
        _tft.setTextColor(TFT_WHITE); // ตัวหนังสือขาวบนพื้นแดง
        _tft.print("Failed!");
        
        delay(1500);
        
        // คืนค่าหน้าจอเดิม (ถมดำ + วาดเส้นขอบใหม่ + พิมพ์รหัสเดิม)
        _tft.fillRect(10, 65, 230, 30, COLOR_BG);   // ล้างดำ
        _tft.drawRect(10, 65, 230, 30, TFT_WHITE);  // วาดขอบคืน
        _tft.setFreeFont(FMB12);
        _tft.setTextColor(TFT_WHITE, COLOR_BG);
        _tft.setCursor(15, 86); 
        _tft.print(_enteredPassword);
     }
     return;
  }

  // กำหนดค่าพิกัดคีย์บอร์ด (ต้องตรงกับหน้า drawKeyboard)
  int startY = 100; 
  int keyW = 30; int keyH = 26; int gap = 2;

  // 2. ปุ่ม Shift
  if (isButtonPressed(x, y, 2, startY + 3*(keyH+gap), 45, keyH)) {
      _isShift = !_isShift;
      drawKeyboard(); 
      return;
  }

  // 3. ปุ่ม Spacebar
  if (isButtonPressed(x, y, 80, startY + 4*(keyH+gap), 160, keyH)) {
      _enteredPassword += " ";
      // อัปเดตหน้าจอ (ล้างเฉพาะข้างใน + พิมพ์ใหม่)
      _tft.fillRect(11, 66, 228, 28, COLOR_BG);
      _tft.setFreeFont(FMB12);
      _tft.setTextColor(TFT_WHITE, COLOR_BG);
      _tft.setCursor(15, 86); 
      _tft.print(_enteredPassword);
      return;
  }

  // 4. ปุ่มตัวอักษร
  String rows[] = { "1234567890", _isShift ? "QWERTYUIOP" : "qwertyuiop", _isShift ? "ASDFGHJKL" : "asdfghjkl", _isShift ? "ZXCVBNM" : "zxcvbnm" };
  int offsets[] = {2, 2, 15, 50};
  int lens[] = {10, 10, 9, 7};

  for (int r=0; r<4; r++) {
      int yRow = startY + r*(keyH+gap);
      if (y >= yRow && y <= yRow+keyH) { 
          for (int c=0; c<lens[r]; c++) {
              int xKey = offsets[r] + c*(keyW+gap);
              if (x >= xKey && x <= xKey+keyW) {
                  _enteredPassword += rows[r][c];
                  // อัปเดตหน้าจอ
                  _tft.fillRect(11, 66, 228, 28, COLOR_BG);
                  _tft.setFreeFont(FMB12); 
                  _tft.setTextColor(TFT_WHITE, COLOR_BG);
                  _tft.setCursor(15, 86); 
                  _tft.print(_enteredPassword);
                  return;
              }
          }
      }
  }

  // 5. ปุ่ม Delete
  if (isButtonPressed(x, y, 265, startY + 3*(keyH+gap), 52, keyH)) {
      drawButtonEffect(265, startY + 3*(keyH+gap), 52, keyH); // เพิ่ม Effect ให้รู้ว่ากดติด
      if (_enteredPassword.length() > 0) {
          _enteredPassword.remove(_enteredPassword.length()-1);
          // อัปเดตหน้าจอ
          _tft.fillRect(11, 66, 228, 28, COLOR_BG);
          _tft.setFreeFont(FMB12); 
          _tft.setTextColor(TFT_WHITE, COLOR_BG);
          _tft.setCursor(15, 86); 
          _tft.print(_enteredPassword);
      }
  }
}

void SystemGUI::drawLoRaStatus() {
  _tft.fillScreen(COLOR_BG);
  drawHeader(true); // มีปุ่ม Back
  
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  
  _tft.setCursor(10, 70); _tft.print("LoRaWAN Status");
  _tft.drawLine(10, 75, 220, 75, COLOR_TEXT);

  // Status
  int y = 110;
  _tft.setFreeFont(FMB9);
  _tft.setTextColor(TFT_LIGHTGREY, COLOR_BG);
  _tft.setCursor(20, y); _tft.print("State:");
  
  _tft.setFreeFont(FMB12);
  if (_loraStatus == "Joined") _tft.setTextColor(TFT_GREEN, COLOR_BG);
  else if (_loraStatus == "Failed") _tft.setTextColor(TFT_RED, COLOR_BG);
  else _tft.setTextColor(TFT_YELLOW, COLOR_BG);
  
  _tft.setCursor(100, y); _tft.print(_loraStatus);

  // RSSI
  y += 40;
  _tft.setFreeFont(FMB9);
  _tft.setTextColor(TFT_LIGHTGREY, COLOR_BG);
  _tft.setCursor(20, y); _tft.print("Signal:");
  
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  _tft.setCursor(100, y); 
  if (_loraRSSI == -999) _tft.print("-");
  else { _tft.print(_loraRSSI); _tft.print(" dBm"); }

  // Last Sent
  y += 40;
  _tft.setFreeFont(FMB9);
  _tft.setTextColor(TFT_LIGHTGREY, COLOR_BG);
  _tft.setCursor(20, y); _tft.print("Last Tx:");
  
  _tft.setFreeFont(FMB12);
  _tft.setTextColor(COLOR_TEXT, COLOR_BG);
  _tft.setCursor(100, y); _tft.print(_loraLastSent);

  // Info Box
  y += 50;
  _tft.drawRect(10, y, 300, 60, TFT_DARKGREY);
  _tft.setFreeFont(FMB9);
  _tft.setTextColor(TFT_LIGHTGREY, COLOR_BG);
  _tft.setCursor(20, y+25); _tft.print("Region: AS923");
  _tft.setCursor(20, y+48); _tft.print("Provider: TTN");
}