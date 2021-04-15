#include <CircusESP32Lib.h> // Biblioteket til Circus of Things.

const int heater_ON = 27; // varmeovn på simulert med grønn led. 
const int heater_OFF = 26; // varmeovn av simulert med rød led.



const int ON = HIGH;
const int OFF = LOW;
float room_temp = 23;
float heater_Temperature = 22;

char ssid[] = "***********"; // Navn apå nettverk.
char password[] = "*********"; // Passord til nettverk.
char server[] = "www.circusofthings.com"; // Her ligger serveren.
char temperature_key[] = "25316"; // Nøkkel for signalet på CoT.
char heater_temperature_key[] = "32331"; // Nøkkel for signalet på CoT.
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0ODkwIn0.HRqG5DABkLgWQoQzRgj8RrnLVtWLYP3o7c-Ycpksl7o"; // Presonlig nøkkel for kontoen.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ssid, og passord inn.

void setup() {
    circusESP32.begin();
    Serial.begin(115200); 
    pinMode(heater_ON, OUTPUT); 
    pinMode(heater_OFF, OUTPUT);
}

void loop() {
  heater_Temperature = circusESP32.read(heater_temperature_key,token); //leser av ønsket temperatur justeres av kontroller
  room_temp = circusESP32.read(temperature_key,token); //leser av romtemperatur
  if (room_temp >= heater_Temperature){ //samenligner romtemp med ønsket temp
    digitalWrite(heater_ON, ON);  // led simulerer varmeov på varmeovn på
    digitalWrite(heater_OFF, OFF); 
    delay(1000);
  }
  else{
    digitalWrite(heater_ON, OFF); //led simulerer varmeovn av
    digitalWrite(heater_OFF, ON);
    delay(1000);
    
  }
}
