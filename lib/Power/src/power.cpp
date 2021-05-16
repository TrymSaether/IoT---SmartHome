#include "Power.h"


// ------------ POWER --------------
Power::Power(String n, float kw):name{n}, kW{kw}{
    name = n;
    kW = kw;
    sum += kw; 
}
// ADD
void Power::setPrice(float pw){ // Setter pris per kilowatttime. Kan bestemmes av bruker
    price = pw;
}

float Power::getkW()const{
    return(kW);
}
float Power::getW(){
    return(kW*1000);
}



// GET
String Power::getName()const{
    return(name);
}

float Power::getSum(){
    return(sum);
}
int Power::pNow(){ // Nå
    if(price == 0){
        Serial.println("Price is not set");// Debugging 
    }
    return(sum*price);
}

int Power::pMonth(){ // Måned
    return(24*30*sum*price);
} 
int Power::pYear(){ // År
    return(24*30*365*sum*price);
}


// ---------------- SOLAR ------------------
// KONSTRUKTØR
Solar::Solar(Power* sw): power{sw}{}

void Solar::setAngle(int az){
    azimuth = az;
    angle = az - incline;
}

void Solar::solarPrint(){
    Serial.print("Solar azimuth now: ");
    Serial.println(azimuth);
    Serial.print("Changing solarpanel's angle to: "); // Printer - debug.
    Serial.println(angle);
}
// Skriver til solcellepanelet
void Solar::solarWrite(){
    if(angle<200) // når den er innenfor 180 grader, justert for helling.
    {
        panel.write(angle);
    }
}
// --- Kostnad solcellepanel ----
// Henter vinkel
const int Solar::getAngle()const{
    return(angle);
}