#ifndef SYSTEMGUI_H
#define SYSTEMGUI_H

#include <TFT_eSPI.h>
#include <Adafruit_FT6206.h>
#include <WiFi.h>
#include "Settings.h"
#include "Icons.h"
#include "Config.h"

enum SystemState {
  SCREEN_DASHBOARD,
  SCREEN_RANGE_LIST,
  SCREEN_SETTING,
  SCREEN_CALIBRATION,
  SCREEN_EDIT_RANGE,
  SCREEN_WIFI_SCAN,
  SCREEN_WIFI_PASSWORD,
  SCREEN_LORA_STATUS // <--- 1. เพิ่มบรรทัดนี้ (อย่าลืม comma ข้างบน)
};

class SystemGUI {
  public:
    SystemGUI();
    void init();
    void loop();
    void updateValues(float temp, float ph, float doVal);
    void updateLoRaStatus(String status, int rssi, String lastSent);
    
    void showError(String msg);

    AppSettings settings; 

  private:
    TFT_eSPI _tft = TFT_eSPI();
    int _currentState;
    
    // ตัวแปรเดิม
    float _cachePH, _cacheDO, _cacheTemp;
    bool _hasData;
    unsigned long _lastBlinkTime;
    bool _blinkState;

    // Edit Range
    float* _ptrMin; float* _ptrMax; float _tempMin; float _tempMax;
    float _limitAbsMin; float _limitAbsMax; String _editTitle; 

    // Wifi Variables 
    int _wifiScrollOffset = 0;
    int _wifiCount = -1; 
    int* _wifiSortedIndices = nullptr; 
    bool _isShift = true;
    String _selectedSSID = "";         
    String _enteredPassword = "";      
    
    // <--- 3. เพิ่มตัวแปรเก็บค่าสถานะ LoRa ตรงนี้
    String _loraStatus = "Connecting...";
    int _loraRSSI = -999;
    String _loraLastSent = "-";

    // Wifi Functions
    void drawWifiList(); 
    void drawWifiPasswordScreen();     
    void drawKeyboard();               
    void handleWifiPasswordTouch(uint16_t x, uint16_t y);

    // Core Functions
    void changeScreen(int newState);
    void handleTouch(uint16_t x, uint16_t y);
    bool handleGlobalNavbar(uint16_t x, uint16_t y);
    bool handleBackButton(uint16_t x, uint16_t y);
    bool isButtonPressed(uint16_t x, uint16_t y, int bx, int by, int bw, int bh);
    void drawButtonEffect(int x, int y, int w, int h);
    void drawHeader(bool showBackBtn);
    void flashListRow(int textY, String text);

    // Screen Drawing
    void drawDashboard();
    void updateDashboardValues();
    void drawRangeList();
    void drawSetting();
    void drawCalibration();
    void drawWifiScan();
    void drawLoRaStatus();

    // Edit Logic
    void openRangeEdit(String title, float* minPtr, float* maxPtr, float absMin, float absMax);
    void drawRangeEditScreen();
    void handleRangeEditTouch(uint16_t x, uint16_t y);
};

#endif