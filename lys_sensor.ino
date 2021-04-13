const int light_sesor = 25; //pin den er koblet på

void setup() {
Serial.begin(9600);
pinMode(light_sesor, INPUT); //setter lyssensoren som input
}

void loop() {
int analogvalue = analogRead(light_sesor); //leser av lyssensoren
Serial.println(analogvalue); // printer verdien av lyssensoren.
delay(1000);
}
