#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

class ButtonManager {
private:
    // // ใช้ static pointer เพื่อให้ฟังก์ชัน Interrupt (ISR) ภายนอกสามารถอ้างอิงถึง Object นี้ได้
    // static ButtonManager* _instance;
    // volatile bool _btnStates[4]; // เก็บสถานะปุ่มทั้ง 4 ปุ่ม

    bool _btnStates[4];        
    bool _lastButtonState[4];  
    uint8_t _pins[4] = {5, 6, 7, 8}; 

public:
    ButtonManager();
    void begin();

    // ฟังก์ชันนี้จะถูกเรียกซ้ำๆ ใน loop() เพื่อตรวจจับการกดแทน Interrupt
    void update(); 
    
    bool isPressed(uint8_t index); 
    void clearState(uint8_t index); 
    
    // // ฟังก์ชันสำหรับให้ ISR เรียกใช้ (ต้องเป็น static หรือ public)
    // void handleISR_D();
    // void handleISR_B();
};

#endif