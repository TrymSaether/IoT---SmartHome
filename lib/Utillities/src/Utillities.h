#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

// Pins, for å forenkle bruken av de i funksjoner
enum Pins
{
    ledPin = 15, // ref ledpin (Rød) 
    ledRPin = 15,
    ledGPin = 2,
    ledBPin = 0,
    heaterPin = 60,
    buttonPin1 = 18,
    buttonPin2 = 19,
    ceilPin1 = 48,
    ceilPin2 = 49,
    ceilPinEN1 = 50,
    curtPin1 = 51,
    curtPin2 = 52,
    curtEN1 = 53,
    solarPanPin = 4,
    // ---- READ -----
    // * Sensorer
    ldrPin = 32,
    magPin = 35,
    dhtPin = 33,
    trigPin = 14,
    echoPin = 27
    
};


void curtainState(int buttonState);
int distanceSensor();
void ceilingState(int ceilingSpeed);
void lightStates(int ledSlider);
void heatertempState(int roomTemp);
void callback();
void callbackSub(char *topic, byte *message, unsigned int length);
void reconnect();
void coronaDistanceChecker(int distanceNow);
