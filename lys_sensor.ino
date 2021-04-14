#include <CircusESP32Lib.h> // Biblioteket til Circus of Things.
const int light_sesor = 25; //pin den er koblet på

char ssid[] = "*********"; // Navn apå nettverk.
char password[] = "**********"; // Passord til nettverk.
char server[] = "www.circusofthings.com"; // Her ligger serveren.
char light_sesor_key[] = "25316"; // Nøkkel for signalet på CoT.
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0ODkwIn0.HRqG5DABkLgWQoQzRgj8RrnLVtWLYP3o7c-Ycpksl7o"; // Presonlig nøkkel for kontoen.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ssid, og passord inn.

void setup() {
circusESP32.begin();
Serial.begin(115200); 
pinMode(light_sesor, INPUT); //setter lyssensoren som input
}

void loop() {
int analogvalue = map(analogRead(light_sesor),0,1023,0,255); //leser av lyssensoren
Serial.println(analogvalue); // printer verdien av lyssensoren.
delay(1000);
}
