#include <analogWrite.h>

enum States {ceiling_fan,
 ceiling_fan_ON,
 ceiling_fan_OFF};
enum States state;

const int buttonPin = 15;

int count = 0;
int ON = 1;
int OFF = 0;
int button_state = LOW;

char ssid[] = "*********"; // Navn apå nettverk.
char password[] = "**********"; // Passord til nettverk.
char server[] = "www.circusofthings.com"; // Her ligger serveren.
char ceiling_fan_key[] = "25316"; // Nøkkel for signalet på CoT.
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0ODkwIn0.HRqG5DABkLgWQoQzRgj8RrnLVtWLYP3o7c-Ycpksl7o"; // Presonlig nøkkel for kontoen.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ssid, og passord inn.

void setup()
{
circusESP32.begin();  
state = ceiling_fan;
pinMode(buttonPin, INPUT);
Serial.begin(9600);

}
void loop() {
switch (state) {
 case ceiling_fan:
      button_state = digitalRead(buttonPin);
      if(button_state == HIGH) {
        if (count == 0){
          state = ceiling_fan_ON;
        }
        else{
          state = ceiling_fan_OFF;
        }
      }
      break;

 case ceiling_fan_ON:
      Serial.println("ON");
      count = 1;
      circusESP32.write(ceiling_fan_key, ON, token);
      state = ceiling_fan;
      delay(1000);
      break;

 case ceiling_fan_OFF:
      Serial.println("OFF");
      count = 0;
      circusESP32.write(ceiling_fan_key, OFF, token);
      state = ceiling_fan;
      delay(1000);
      break;
}
}
