#include <Arduino.h>
#include "ESP32Servo.h"


#define KWHTOW 3600 
#define HOUR_DAY 24
#define MONTH 30
/*
const int area = 150; // kvm areal av taket
const float factor = 0.75; // pga kostnaden til solcellepanel vil være redusert med 25%
const float avgSun = 4.3; // Gjennomsnitt antall timer solen er oppe i Norge (Oslo)
const float kWPerSqm = 0.148; // 15-20% av 1000watt
*/
// ------- POWER KLASSE --------
// Energiklasse
class Power
{
private:
    String name; // Navn på element
    float kW; // KiloWattTime
    float sum; // energiforbruk sum
    float price; // Pris per kwh
    float* kollektiv;
public:
    Power(String n, float kw);
    float getSum();
    String getName()const;
    float getkW()const;
    float getW();
    void setPrice(float pw); // Sett pris "p"-Now/Month/year, per kW
    int pNow(); // kostnader: Nå 
    int pMonth();
    int pYear(); //.
    ~Power(){};
};


// ------- SOLAR KLASSE --------
class Solar
{
public:
    Servo panel;
private: 
    const int incline = 20; //  grader helning av tak
    int angle; // Vinkelen ut til solcellepanel
    int azimuth; // Azimuth til solen
    Power* power;
public:
    Solar() = default;
    Solar(Power* sw = nullptr);
    void setAngle(int az);
    void solarWrite();
    void solarPrint();
    const int getAngle()const;
    ~Solar(){};
};



