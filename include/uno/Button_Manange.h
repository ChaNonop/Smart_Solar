#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

class ButtonManager {
private:
    // ใช้ static pointer เพื่อให้ฟังก์ชัน Interrupt (ISR) ภายนอกสามารถอ้างอิงถึง Object นี้ได้
    static ButtonManager* _instance;
    volatile bool _btnStates[4]; // เก็บสถานะปุ่มทั้ง 4 ปุ่ม

public:
    ButtonManager();
    void begin();
    
    bool isPressed(uint8_t index); // เช็คว่าปุ่มที่ 0-3 ถูกกดหรือไม่
    void clearState(uint8_t index); // ล้างสถานะเมื่ออ่านค่าไปแล้ว
    
    // ฟังก์ชันสำหรับให้ ISR เรียกใช้ (ต้องเป็น static หรือ public)
    void handleISR_D();
    void handleISR_B();
};

#endif