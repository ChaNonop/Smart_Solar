#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class SDCard {
private:
    const int chipSelect = 4;
    File dataFile;

public:
    SDCard();
    void begin();
}

#endif