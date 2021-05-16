// bibliotek
#include <Arduino.h>
#include <ESP32servo.h>
#include "Utillities.h"


// Variabler
#define CORONADISTANCE 2     // Anbefalt 2m avstand fra FHI.
#define CURTAININTERVAL 2000 // Tiden for at gardin går opp/ned.
#define CURTAINPOWER 30      // styrken på gardin DC-motor for å få riktig styrke ifht gardinintervall

// --------- Funksjoner ------------
// mapper en bestemt lengde fra 0 - length
int mapStandard(int nmval, int length)
{
    int val = map(nmval, 0, length, 0, 4095);
    return (val);
}

// Distansensor
float distanceSensor()
{
    float duration, distance;           // tid og lengde (cm)
    duration = pulseIn(echoPin, HIGH);  // Leser signalet fra sensoren
    distance = ((duration / 2) / 29.1); // Regner ut distansen...
    return (distance / 10);
}

// Tak/vanlig vifte hastighet
void ceilingFan(int subVal) // subscribe inn
{
    if (subVal > 0) // Over null
    {
        digitalWrite(enable1Pin, LOW);
        digitalWrite(enable2Pin, HIGH);
        analogWrite(ceilingFanPin, subVal);
    }
    else
    {
        digitalWrite(enable1Pin, LOW);
        digitalWrite(enable2Pin, LOW);
        analogWrite(ceilingFanPin, LOW);
    }
}

// Gardin opp/ned fra dashboard
void curtainState(bool subVal)
{
    long now = millis();
    while (millis() < now + CURTAININTERVAL)
    {
        analogWrite(curtainPin, CURTAINPOWER);
        now = millis();
        if (subVal)
        { // OPP
            digitalWrite(enable1Pin, subVal);
            digitalWrite(enable2Pin, !subVal);
        }
        else if (!subVal)
        { // NED
            digitalWrite(enable1Pin, !subVal);
            digitalWrite(enable2Pin, subVal);
        }
        else
        { // Debug
            Serial.print("Error -> Curtains");
        }
        // default, setter pin til lav.
        digitalWrite(enable1Pin, LOW);
        digitalWrite(enable2Pin, LOW);
    }
}

void lightStates(String msgTemp)
{
    // Lys med smartfunksjon
    if(msgTemp == "smart")
    {
        short int ldrSensor = map(analogRead(ldrPin),0,4095,4095,0); // Leser fotoresistor
        analogWrite(ledBPin,ldrSensor); // skriver den ut til lyset slik at den dimmes motsatt med den.
    }
    else if(msgTemp == "true")
    {
      digitalWrite(ledBPin, HIGH);
    }
    else if(msgTemp == "false"){
      digitalWrite(ledBPin, HIGH);
    }
}

void heaterState(int subVal, int roomTemp) // OBS GJØRGJØR!
{
    if (subVal > 0 && subVal <= roomTemp)
    {
        digitalWrite(heaterPin, HIGH);
    }
    else
    {
        digitalWrite(heaterPin, LOW);
    }
}

// Tester om noen er nærmere enn 2 meter, i henhold til Koronareglene.
bool coronaCheckDistance(int distanceNow)
{
    if (distanceNow < CORONADISTANCE)
    {
        digitalWrite(ledRPin, HIGH);
        return (true);
    }
    else
    {
        digitalWrite(ledRPin, LOW);
        return (false);
    }
}

void callback()
{
    //placeholder callback funksjon
}