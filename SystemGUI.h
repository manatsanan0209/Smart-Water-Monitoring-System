#ifndef SYSTEM_GUI_H
#define SYSTEM_GUI_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

class SystemGUI {
  public:
    // รับค่าขาที่ใช้ต่อจอ
    SystemGUI(int8_t cs, int8_t dc, int8_t rst);

    // เริ่มต้นจอ หรือ รีเซ็ตจอ (ใช้สำหรับ Recovery ได้ด้วย)
    void init();

    // อัปเดตค่าขึ้นหน้าจอ
    void updateValues(float temp, float ph);

    // แสดง Error ด้านล่างจอ
    void showError(String msg);

  private:
    Adafruit_ILI9341 _tft;
    
    // ฟังก์ชันวาดหน้าตาพื้นฐาน (เส้น, หัวข้อ)
    void drawInterface();
};

#endif