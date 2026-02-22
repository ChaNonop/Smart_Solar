#include "esp/sd_card.h"

SDCard::SDCard() {
    
}

void SDCard::begin() {
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        return;
    }
}