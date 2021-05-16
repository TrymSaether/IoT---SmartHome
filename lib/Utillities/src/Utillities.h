#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

// Pins, for å forenkle bruken av de i funksjoner
enum Pins
{
    // ----- WRITE ------
    ledPin = 4, // referanse pinne ved fravær av RGB-led. 
    ledRPin = 4,
    ledGPin = 16,
    ledBPin = 17, 
    heaterPin = 60,
    ceilingFanPin = 48,
    curtainPin = 51,
    enable1Pin = 51, // Bruker samme enablePin på begge DC motorene
    enable2Pin = 53,
    servoPin = 15, 
    // ---- READ -----
    button1Pin = 19,
    button2Pin = 19,
    ldrPin = 32, // Lightdependent resistor pin
    magnetHallPin = 35, // Magnetisk halleffekt pin
    dhtPin = 33,  // temp, luftfuktighetpin
    trigPin = 14, // Ultrasonic beam sensor: 
    echoPin = 27 //sender og mottaker pin for
};

void reconnect();
void callback();
void callbackMQTT(char *topic, byte *message, unsigned int length);
void curtainState(bool subVal);
void ceilingFan(int subVal);
void lightStates(String ldrTemp);
void heaterState(int subVal, int roomTemp);
bool coronaCheckDistance(int distance);
int mapStandard(int val, int length); 
int dcControl(int subVal, Pins p);
float distanceSensor();