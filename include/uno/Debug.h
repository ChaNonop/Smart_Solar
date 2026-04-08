#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

// ==========================================
// ตั้งค่าเป็น 1 เมื่อต่อคอมพิวเตอร์เพื่อดูค่า
// ตั้งค่าเป็น 0 เมื่อนำไปต่อแผงใช้งานจริง 
// ==========================================
#define ENABLE_DEBUG 0
#if ENABLE_DEBUG
  #define DEBUG_BEGIN(baud) Serial.begin(baud)
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_BEGIN(baud)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#endif