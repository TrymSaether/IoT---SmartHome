const int potPin = 12;

int max_temp = 34;
int min_temp = 0;
int max_raw_input = 4096;
int min_raw_input = 0;

char ssid[] = "*******"; // Navn apå nettverk.
char password[] = "*********"; // Passord til nettverk.
char server[] = "www.circusofthings.com"; // Her ligger serveren.
char heater_temperature_key[] = "25316"; // Nøkkel for signalet på CoT.
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0ODkwIn0.HRqG5DABkLgWQoQzRgj8RrnLVtWLYP3o7c-Ycpksl7o"; // Presonlig nøkkel for kontoen.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ssid, og passord inn.

void setup() {
    Serial.begin(115200);
    circusESP32.begin();
}

void loop() { 
    int heater_temp = map(analogRead(potPin), min_raw_input, max_raw_input, min_temp, max_temp);
    circusESP32.write(heater_temperature_key,heater_temp,token);
    delay(500);
}
