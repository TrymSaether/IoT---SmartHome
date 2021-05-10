#include <Arduino.h>
#include <ESP32servo.h>
#include "time.h"
#include "Utillities.h"

// Variabler
// ----- Funksjoner ------
int distanceSensor(){
    float duration, distance;          // tid og lengde (cm)
    duration = pulseIn(echoPin, HIGH); // Leser signalet fra sensoren
    // konverterer tid til distanse.
    distance = (duration / 2) / 29.1; // Regner ut distansen...
    return (distance);
}

void ceilingState(int ceilingSpeed)
{
    if (ceilingSpeed > 2)
    {
        digitalWrite(ceilPin1, LOW);
        digitalWrite(ceilPin2, HIGH);
        analogWrite(ceilPinEN1, ceilingSpeed);
    }
    else
    {
        digitalWrite(ceilPin1, LOW);
        digitalWrite(ceilPin2, LOW);
        analogWrite(ceilPinEN1, 0);
    }
}

bool curtainState(bool motorState)
{
    const int curtainInterval = 2000;
    long now = millis();
    if (millis() < now + curtainInterval)
    {
        digitalWrite(curtPin1, motorState);
        digitalWrite(curtPin2, !motorState);
        analogWrite(curtEN1, 30);
        motorState = !motorState;
        return(motorState);
    }
    else
    {
        digitalWrite(curtPin1, LOW);
        digitalWrite(curtPin2, LOW);
        analogWrite(curtEN1, 0);
        return(0);
    }
}

void lightStates(int ledSlider)
{
    if (ledSlider > 0)
    {
        analogWrite(ledPin, ledSlider);
    }
}

void heatertempState(int roomTemp) // OBS GJØRGJØR!
{

}

void coronaDistanceChecker(int distanceNow){
  const int coronaDistance = 200;
    if(distanceNow<coronaDistance){
      digitalWrite(ledPin,HIGH);
    }
    else if(distanceNow>coronaDistance){
      digitalWrite(ledPin, LOW);
    }
}
void callback()
{
    //placeholder callback function
}