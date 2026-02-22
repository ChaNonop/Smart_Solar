#include "uno/Button_Manange.h"

// กำหนดค่าเริ่มต้นให้กับ Pointer
ButtonManager* ButtonManager::_instance = nullptr;

ButtonManager::ButtonManager() {
    _instance = this;
    for(int i = 0; i < 4; i++) {
        _btnStates[i] = false;
    }
}

void ButtonManager::begin() {
    // กำหนดขา D5, D6, D7, D8 เป็น INPUT_PULLUP
    pinMode(5, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);

    // เปิดใช้งาน Interrupt ระดับ Hardware
    PCICR |= (1 << PCIE2) | (1 << PCIE0);
    PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23); // ขา D5-D7
    PCMSK0 |= (1 << PCINT0);  // ขา D8
}

bool ButtonManager::isPressed(uint8_t index) {
    if (index >= 4) return false;
    return _btnStates[index];
}

void ButtonManager::clearState(uint8_t index) {
    if (index < 4) _btnStates[index] = false;
}

// เมื่อเกิด Interrupt กลุ่ม D (ขา 5,6,7) จะเข้ามาทำงานฟังก์ชันนี้
void ButtonManager::handleISR_D() {
    if(digitalRead(5) == LOW) _btnStates[0] = true;
    if(digitalRead(6) == LOW) _btnStates[1] = true;
    if(digitalRead(7) == LOW) _btnStates[2] = true;
}

// เมื่อเกิด Interrupt กลุ่ม B (ขา 8) จะเข้ามาทำงานฟังก์ชันนี้
void ButtonManager::handleISR_B() {
    if(digitalRead(8) == LOW) _btnStates[3] = true;
}

// ฟังก์ชัน Interrupt พื้นฐานของ Arduino 
ISR(PCINT2_vect) {
    if (ButtonManager::_instance) {
        ButtonManager::_instance->handleISR_D();
    }
}

ISR(PCINT0_vect) {
    if (ButtonManager::_instance) {
        ButtonManager::_instance->handleISR_B();
    }
}