#include "Utillities.h"

void solarpanelState(int solarAzimuth){
    solarAzimuth = solarAzimuth - 20;
    analogWrite(solarPanPin, solarAzimuth);
}
