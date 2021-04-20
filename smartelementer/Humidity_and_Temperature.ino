#include <CircusESP32Lib.h> // Biblioteket til Circus of Things.
#include "DHT.h"            // Biblioteket for å bruke temperatur og luftfuktighet

#define DHTPIN 4     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "*************"; // Navn apå nettverk.
char password[] = "***********"; // Passord til nettverk.
char server[] = "www.circusofthings.com"; // Her ligger serveren.
char Temperature_key[] = "25316"; // Nøkkel for signalet på CoT.
char Humidity_key[] = "7876"; // Nøkkel for signalet på CoT.
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0ODkwIn0.HRqG5DABkLgWQoQzRgj8RrnLVtWLYP3o7c-Ycpksl7o"; // Presonlig nøkkel for kontoen.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ssid, og passord inn.

void setup() {
  Serial.begin(115200);
  dht.begin();
  circusESP32.begin();
}

void loop() {
  delay(2000); //tid mellom hver avlesning

  float Humidity_reading = dht.readHumidity();       //Leser av luftfuktighet
  float Temperature_reading = dht.readTemperature(); //Leser av temperatur i celsius

  circusESP32.write(Temperature_key, Temperature_reading,token); // Laster opp luftfuktighet verdi til CoT
  circusESP32.write(Humidity_key, Humidity_reading,token);       // Laster opp temperatur verdi til CoT
}
