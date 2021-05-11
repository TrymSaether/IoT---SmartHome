// Bibliotek
#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include "time.h"
#include "Utillities.h"
#include "DHT.h"
#include "SolarAngle.h"
#include "Power.h"
#include "Print.h"

// ----- Definerte variabler" -----
#define Threshold 40 // Toleranse for touchpinoppvåkning.
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */
// ----- Variabler ------
// Wifi:
const char *ssid =  "Trym sin iPhone"; // "Telenor9381sky";
const char *password = "eplepai1"; //"xtpevmzjukkwq";
const char *mqtt_server = "172.20.10.5"; // "10.0.0.66";// MQTT signalbehandling
const char ID[] = "Trym";
WiFiClient espClient; // Esp wifioppkobling
PubSubClient client(espClient); // klient for sending av esp32 mqtt signal til RPi

// Tid:
const char *ntpServer = "pool.ntp.org"; // henter tid - nå i div format.
const long gmtOffset_sec = 3600; 
const int daylightOffset_sec = 3600;
const int delayInterval = 5000; // delayintervall 5sek.

// Diverse
long lastMsg = 0; // Midlertidig variabel for "delay"
float applianceStates[6] = {1.2, 0.6, 0.25, 0.1, 0, 1.1}; // Array for strømforbruk; 0: kjøkken, 1: bad, 2: stue, 3: soverom, 4: Solcelle 5: Passiv elementer
bool buttonState; // Fysisk pullupknapp
signed int sensorTemp;
const int degreeAdjRoof = 20;
RTC_DATA_ATTR int bootCount = 0; // antall ganger booted opp.
DHT dht(dhtPin, DHT11); // Temp og luftfuktighet. sensor.
Servo solarPanelAngle; // Servo til solcellepanel.

// ----------- SETUP -------------
void setup()
{
  // pinModes.
  pinMode(ledPin, OUTPUT);
  pinMode(ledGPin, OUTPUT);
  pinMode(ledBPin, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(magPin, INPUT);
  pinMode(ldrPin, INPUT);
  Serial.begin(115200); // Seriemonitor
  dht.begin(); // sensorer

  solarPanelAngle.attach(solarPanPin); // solcellepanelservo.
  ++bootCount; // Debugging
  Serial.println("Boot number: " + String(bootCount));
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);           // WiFi tilkobling.
  while (WiFi.status() != WL_CONNECTED) // Wifi statussjekk/feilmeldinger
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883); // MQTT broker
  client.setCallback(callbackSub); // Callback funksjon for signalbehandling MQTT.
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // Justering av tid, for +gmt og sommertid.
  printLocalTime();  // Printer tid nå.
  // Interuptfunksjoner.
  // touchAttachInterrupt(GPIO_NUM_1, callback, Threshold); // esp våkner når gpio1 berøres. threshold satt til touchRead(2) testet verdi.
  
}
void loop()
{
  //  -- Test Wifi --
  if (!client.connected())
  {
    reconnect(); // Prøver på nytt.
  }
  client.loop(); // Looper mqtt brokeren/en måte for å starte callbackfunksjonen.
  // --- homeHub ---
  buttonState = digitalRead(buttonPin1); // Leser knapp for booking av dass.
  unsigned long now = millis();
  if (now - lastMsg > delayInterval) // delay  
  {
    lastMsg = now;
    // * Sensorverdier
    float h = dht.readHumidity(); // Fuktighet
    float t = dht.readTemperature(); // Temperatur
    float l = analogRead(ldrPin);
    float cd = distanceSensor(); // Koronadistanse
    float m = digitalRead(magPin);
    Serial.print("Magnetsensor: ");
    Serial.println(m);
    // * Beregner totalt energiforbruk
    float p = powerContribution(applianceStates, 1); 
    printSensorData(h,t,p,cd,l); // Printfunksjon
    // coronaDistanceChecker(cd); // Distansefunksjon
    // * Omgjør sensor/forbruk int, float -> "string/chars"
    char ldrString[8];
    char tempString[8];
    char humString[8];
    char powString[8];
    char solarpowString[8];
    dtostrf(t, 1, 2, tempString);
    dtostrf(h, 1, 2, humString);
    dtostrf(p, 1, 2, powString);
    dtostrf(l, 1, 2, ldrString);
    dtostrf(applianceStates[5],1,2,solarpowString);
    // * Sender data til Node Red topics
    client.publish("homeHub/sensors/temperature", tempString);
    client.publish("homeHub/sensors/humidity", humString);
    client.publish("homeHub/power/currentpower", powString);
    //client.publish("homeHub/power/solarpower",solarpowString);
    client.publish("homeHub/sensors/lightsensor",ldrString);
    // * Booker toalett i xSekund med knapp.
    if(buttonState == true)
    {
      client.publish("homeHub/booking/bathroom", "booked"); //homeHub/booking/bathroom er token for booking bad og booked siar at det er opptattpå badet.
      client.publish("homeHub/booking/ID", ID); // Sender ID på hvem som booker
      Serial.println("Bathroom is booked by: ");
      Serial.print(ID);
    }
  }
}


// MQTT-temaer og wifitilkobling ved avbrudd.
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("homeHub/sensors/");
      client.subscribe("homeHub/booking/bathroom1");
      client.subscribe("homeHub/room/light");
      client.subscribe("homeHub/room/ceilingspeed");
      client.subscribe("homeHub/room/heater");
      client.subscribe("homeHub/power/azimuth");
      client.subscribe("homeHub/power/sleep");
    }
    else
    {// Looper 
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// --- Callback ---
/*
peker til topic i N.R(les: Node Red), som leser meldingen fra hvert topic 
som en String og bestemmer hvilke funksjoner/funksjonalitet de skal ha. F.eks. 
ved knappetrykk på "webserveren"/API til kollektivet skal utføre X operasjon på/i ESP32.
*/
void callbackSub(char *topic, byte *message, unsigned int length)
{
  // * Printer i seriemonitor 
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);// topic fra N.R når beskjed kommer.
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) // Selve beskjeden fra gjeldende topic
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // ---- NODE RED FUNKSJONER ----
  /* --- Strømforbruk ---
  De aller fleste funksjoner som skrur på komponenter eller funksjonaliter 
  som "bruker" strøm vil øke verdien til det gitt rommet og komponenten i 
  ett array "applianceStates[x]" gitt i kw.
    - Booking kjøkken - 
  Setter lys til blått når noen har booket og bruker kjøkken.
  */
  if (String(topic) == "homeHub/booking/kitchen") 
  {
    Serial.print("Changing output to ");
    if (messageTemp == "true")
    {
      Serial.println("true");
      digitalWrite(ledBPin, HIGH);
      applianceStates[0] += 4.0;
    }
    else if (messageTemp == "false")
    {
      Serial.println("false");
      digitalWrite(ledBPin, LOW);
    }
  }
  // - Booking bad -
  // * Setter lys til rødt når noen har booket og bruker badet.
  else if (String(topic) == "homeHub/booking/bathroom1")
  {
    Serial.print("Changing output to ");
    if (messageTemp == "booked")
    {
      digitalWrite(ledRPin, HIGH);
      applianceStates[1] = 2.8; // Forbruk aktivt bad
    }
    else if (messageTemp == "available")
    {
      client.publish("homeHub/booking/ID", "Available"); // Sender at er bad ledig
      digitalWrite(ledRPin, LOW); 
      digitalWrite(ledGPin, HIGH); 
      digitalWrite(ledBPin, LOW); 
    }
  }
  // - Booking stue -
  else if (String(topic) == "homeHub/booking/livingroom")
  {
    if (messageTemp == "booked")
    {
      applianceStates[2] = 1.85; // Forbrukt aktiv stue
    }
    else if (messageTemp == "available")
    {
      applianceStates[2] = 0.30;
    }
  }
  else if (String(topic) == "homeHub/room/heater"){
    int heaterSettings = messageTemp.toInt();
    int t1 = dht.readTemperature(); // temperatursjekk rom
    bool hsState = true;
    if(heaterSettings > t1 && hsState){
      // digitalWrite(heaterPin, LOW);
      applianceStates[3] -= heaterSettings/20;
      hsState = false;
    }
    else if(heaterSettings < t1 && hsState != true){
      // analogWrite(heaterPin, heaterSettings);
      applianceStates[3] += heaterSettings/20;
      hsState = true;
    }
  }
  else if (String(topic) == "homeHub/room/ceilingspeed")
  {
    bool csState = true;
    int csTemp = messageTemp.toInt();
    if(csTemp == 0 && csState){
      applianceStates[3] -= 0.2;
      csState = false;
    }
    else if(csTemp>0 && csState != true){
      applianceStates[3] += 0.2;
      csState = true;
    }
  }
   else if (String(topic) == "homeHub/sensors/lightsensor")
  {
    sensorTemp = messageTemp.toInt();
  }
   else if (String(topic) == "homeHub/room/light")
  {
    if (messageTemp == "true")
    {
      analogWrite(ledBPin, sensorTemp);
      applianceStates[3] += 0.015;
    }
    else if (messageTemp == "false")
    {
      digitalWrite(ledBPin, LOW);
      applianceStates[3] -= 0.15;
    }
    else if(messageTemp == "trueon"){
      digitalWrite(ledBPin, HIGH);
      applianceStates[3] += 0.15;
    }
  }
  else if (String(topic) == "homeHub/power/azimuth")
  {
    int azimuthAdj = messageTemp.toInt() - degreeAdjRoof;
    Serial.print("Azimuth Angle: ");
    Serial.println(azimuthAdj);
    if(azimuthAdj<=180 && azimuthAdj>=0){
      solarPanelAngle.write(azimuthAdj); // vinkel for solcellepanel, justert med 20 grader
      applianceStates[5] = -10.0;
      char solarpower[8];
      dtostrf(abs(applianceStates[5]),3,1,solarpower);
      client.publish("homeHub/power/solarpower",solarpower);
    }
    else{
      applianceStates[5] = 0;
    }
  }
  else if (String(topic) == "homeHub/power/sleep")
  {
    if (messageTemp == "true")
    {
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // sjekker hvert 5 min om away er
      esp_deep_sleep_start();
    }
    else if(messageTemp == "false"){
      print_wakeup_reason();
      print_wakeup_touchpad();
    }
  }
}
