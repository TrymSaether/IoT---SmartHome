#include <CircusESP32Lib.h> // Biblioteket til Circus of Things.
int light_in_room = 10;   
const int max_brightness = 255; //maks lysstyrke

char ssid[] = "*********"; // Navn apå nettverk.
char password[] = "**********"; // Passord til nettverk.
char server[] = "www.circusofthings.com"; // Her ligger serveren.
char light_sesor_key[] = "25316"; // Nøkkel for signalet på CoT.
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0ODkwIn0.HRqG5DABkLgWQoQzRgj8RrnLVtWLYP3o7c-Ycpksl7o"; // Presonlig nøkkel for kontoen.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ssid, og passord inn.

void setup() {
circusESP32.begin();
Serial.begin(115200); 
pinmode(light_in_room, OUPUT);
}

void loop() {
brightness = (max_brightness - circusESP32.read(temperature_key,token)); // maks lystyrke når det er lav verdi på sensoren koblet opp mot CoT 
analogWrite(light_in_room, brightness);
delay(100);
