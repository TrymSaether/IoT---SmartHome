#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include "time.h"
#include "Utillities.h"
#include "DHT.h"
#include "Power.h"
#include "Print.h"
#include "Matrix.h"

/* | KODE, NAVN & VARIABELSTIL |
1. Alle definerte variabler er i capslock
2. variabelnavn er skrevet i CAMELCASE notasjon.
3. 
*/

// ----- Variabler -----
// Definert:
#define TOUCH_THRESHOLD 300 // Toleranse for touchpinoppvåkning.
#define S_FACTOR 1000000    // micro til sekund
#define TIME_TO_SLEEP 300   // Tid i sek ved oppvåkningssjekk.
// -- Konstanter --
// * WiFi/MQTT
const char *ssid = "Trym sin iPhone";    // "Telenor9381sky";
const char *password = "eplepai1";       //"xtpevmzjukkwq";
const char *mqtt_server = "172.20.10.5"; // "10.0.0.66";// Node-RED server
const char *iD = "Trym";                 // BrukerID

WiFiClient espClient;           // Esp wifioppkobling
PubSubClient client(espClient); // klient for sending av esp32 mqtt signal til RP.

// -- Tid --
const char *ntpServer = "pool.ntp.org"; // henter tid i div format.
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const int delayInterval = 5000; // delayintervall 5sek.

// -- Diverse --
// pullupknapper
bool button1State;
bool button2State;
DHT dht(dhtPin, DHT11);          // Temp og luftfuktighet. sensor.
RTC_DATA_ATTR int bootCount = 0; // Debug bootcount, lagrer det på et spesifikt minne.

// -- tempvariabler --
long lastMsg = 0; // temp for delay, vil bottlenecke etter 60 dager.
short int ldrTemp;
short int roomTemp;
short int heaterTemp;
short int azimuthTemp;
short int ceilingTemp;

// ---- Egendefinerte klasser ----
// - Power -
// Passive komponenter i hovedrom
Power total{"Sum", 1};
Power kitchen{"Pas", 2.2};
Power livingRoom{"Pas", 1.1};
Power bedrooms{"Pas", 6 * 0.4};
Power hallway{"Pas", 4.3};
Power bathroom{"Pas", 33.5};
Power ovn{"Ovn", 1.9};
Power solpow{"sp", -14.2};

// - Solar -
Solar solar{&solpow};
// - Matrix -
int kol = 5;
int rad = 7;
Matrix kollektiv{kol, rad};

// ------------------------------------------------------  SETUP --------------------------------------------------------
void setup()
{

  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);
  solar.panel.attach(servoPin);
  // - pinModes -

  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(magnetHallPin, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(ledRPin, OUTPUT);
  pinMode(ledGPin, OUTPUT);
  pinMode(ledBPin, OUTPUT);
  pinMode(button1Pin, INPUT_PULLDOWN);


  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) // Wifi statussjekk/feil
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883); // MQTT broker
  client.setCallback(callbackMQTT);    // Callback funksjon for signalbehandling MQTT.

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // Justering av tid, for +gmt og sommertid.
  printLocalTime();
  touchAttachInterrupt(GPIO_NUM_1, callback, TOUCH_THRESHOLD); // Interupt: esp våkner når gpio1 berøres.
}
// ------------------------------------------------------  LOOP ---------------------------------------------------------
void loop()
{
  randomSeed(analogRead(0));
  if (!client.connected()) // * WiFi og MQTT
  {
    reconnect();
  }
  client.loop(); // Initialiserer Callback
  // ----------------- homeHub -------------------
  button1State = digitalRead(button1Pin); // Leser knapp for booking av bad.
  Serial.println("Hallo");
  Serial.println(button1State);
  kollektiv.print();
  // button2State = digitalRead(button2Pin);

  // ----------- Intervall ------------
  unsigned long now = millis();
  if (now - lastMsg > delayInterval)
  {
    lastMsg = now;

    //n-----Test-- ----total.setPrice(8);
    float foo1 = solpow.getkW();
    float bar1 = kitchen.getkW();
    int foo2 = total.pYear();
    float foo3 = total.getSum();

    Serial.println(foo1);
    Serial.println(foo2);
    Serial.println(bar1);
    Serial.println(foo3);

    float pw = total.getSum(); // Beregner totalt energiforbruk
    int prc = total.getkW();

   

    
    // --- Sensorer ---
    float h = dht.readHumidity();              // Fuktighet
    float t = dht.readTemperature();           // Temperatur
    float ldr = analogRead(ldrPin);            // LDR - light dependant resistor
    float cd = distanceSensor();               // Koronadistanse
    short int mg = digitalRead(magnetHallPin); // Mapper magnethallsensoren

    // Klargjør home/sunHubdata til å sendes til hovedenhet
    char hStr[8];
    char tStr[8];
    char ldrStr[8];
    char cdStr[8];
    char pwStr[8];
    char prcStr[8];
    char mgStr[3];

    // float/double to const char* / "String"
    dtostrf(h, 2, 1, hStr);
    dtostrf(t, 2, 1, tStr);
    dtostrf(ldr, 4, 1, ldrStr);
    dtostrf(mg, 4, 0, mgStr);
    dtostrf(prc, 5, 1, prcStr);
    dtostrf(pw, 3, 1, pwStr);
    dtostrf(cd,2,1,cdStr);

    // ---- Publish Node-RED ----
    client.publish("homeHub/sensors/temperature", tStr);
    client.publish("homeHub/sensors/humidity", hStr);
    client.publish("homeHub/sensors/lightsensor", ldrStr);
    client.publish("homeHub/sensors/distance", cdStr);
    client.publish("homeHub/power/watt", pwStr);
    client.publish("homeHub/power/price", prcStr);

    // printData(h,t,pw,cd,ldr); // Printfunksjon
    // coronaCheckDistance(cd); // Distansefunksjon
    // Booker toalett i xSekund med knapp.
    // ---- Betingelser ----
    if (button1State)
    {
      client.publish("homeHub/booking/bathroom", "booked");
      client.publish("homeHub/booking/ID", iD); // Sender ID på hvem som booker
      Serial.println("Bathroom is booked by: ");
      Serial.print(iD);
      // Power bathroom{"inUse", 1.2};
    }
  }
}
// ---- Subscribe ----
// MQTT-temaer og wifitilkobling ved avbrudd.
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");

      client.subscribe("homeHub/sensors/");
      client.subscribe("homeHub/booking/bathroom");
      client.subscribe("homeHub/room/light");
      client.subscribe("homeHub/room/ceilingspeed");
      client.subscribe("homeHub/room/heater");
      client.subscribe("homeHub/power/azimuth");
      client.subscribe("homeHub/power/sleep");
    }
    else
    { // ved feil.
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
/*
------- Callback --------
peker til topic i N.R(les: Node Red), som leser meldingen fra hvert topic 
som en String og bestemmer hvilke funksjoner/funksjonalitet de skal ha. F.eks. 
ved knappetrykk på "webserveren"/API til kollektivet skal utføre X operasjon på/i ESP32. */
void callbackMQTT(char *topic, byte *message, unsigned int length)
{
  // * Printer i seriemonitor
  Serial.print("Message arrived on topic: ");
  Serial.print(topic); // topic fra N.R når beskjed kommer.
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) // Selve beskjeden fra gjeldende topic
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  // ------- MQTT -> Node-RED --------
  // --- Booking ---
  // - kjøkken -
  //booker kjøkken nå, etter lys til blått når noen har booket og bruker kjøkken.
  if (!strcmp(topic, "homeHub/booking/kitchen"))
  {
    Serial.print("\nChanging output to ");
    if (messageTemp == "true")
    {
      Serial.println("booked");
      digitalWrite(ledBPin, HIGH);
      Power kitchen1{"Ovn", 1.};
      Power kitchen2{"mikro", 0.9};
    }
    else if (messageTemp == "false")
    {
      Serial.println("available");
      digitalWrite(ledBPin, LOW);
    }
  }
  // - Booking bad -
  // * Setter lys til rødt når noen har booket og bruker badet.
  else if (!strcmp(topic, "homeHub/booking/bathroom"))
  {
    Serial.print("Bathroom: ");
    digitalWrite(ledBPin, LOW);
    if (messageTemp == "booked")
    {
      digitalWrite(ledRPin, HIGH); // Rødt for booket
    }
    else if (messageTemp == "available")
    {
      client.publish("homeHub/booking/ID", "Available"); // Bad ledig
      digitalWrite(ledRPin, LOW);
      digitalWrite(ledGPin, HIGH); // Grønt lys for ledig
    }
  }
  // - stue -
  // * Setter lys til rødt når noen har booket og bruker kjøkkeneet.
  else if (!strcmp(topic, "homeHub/booking/livingroom"))
  {
    Serial.print("Living Room: ");
    if (messageTemp == "booked")
    {
      Serial.println("Booked");
    }
    else if (messageTemp == "available")
    {
      Serial.println("Available");
    }
  }
  else if (!strcmp(topic, "homeHub/bedroom/heater"))
  {
    heaterTemp = messageTemp.toInt();
    roomTemp = dht.readTemperature();
    heaterState(heaterTemp, roomTemp);
  }
  else if (!strcmp(topic, "homeHub/bedroom/ceilingFan"))
  {
    ceilingTemp = messageTemp.toInt();
    ceilingFan(ceilingTemp);
  }
  else if (!strcmp(topic, "homeHub/bedroom/light"))
  {

    if (messageTemp == "true")
    {
      analogWrite(ledBPin, ldrTemp);
    }
    else if (messageTemp == "false")
    {
      digitalWrite(ledBPin, LOW);
    }
    else if (messageTemp == "trueon")
    {
      digitalWrite(ledBPin, HIGH);
    }
  }
  else if (!strcmp(topic, "sunHub/azimuth"))
  {

    char spwStr[8];
    azimuthTemp = messageTemp.toInt();
    client.publish("sunHub/watt", dtostrf(abs(solpow.getkW()), 3, 1, spwStr));
    solar.setAngle(azimuthTemp);
    solar.solarWrite();
    solar.solarPrint();
  }
  else if (!strcmp(topic, "homeHub/sleep")) // Legger ESP32 i hvilemodus, kun to RTC prosess (tid) som kjører
  {
    if (messageTemp == "true")
    {
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * S_FACTOR); // sjekker hvert 5 min om away er
      esp_deep_sleep_start();
    }
    else if (messageTemp == "false")
    {
      print_wakeup_reason();
      print_wakeup_touchpad();
    }
  }
}

void randomMatrix(Matrix mx)
{
  int num;
  for (int i = 0; i < mx.getRows(); i++)
  {
    for (int j = 0; j < mx.getColumns(); j++)
    {
      num = random(40);
      mx.set(i, j, num);
    }
  }
}