#include <WiFi.h>
#include <PubSubClient.h>

const int button = 15; 
const int lamp = 2;
const int lamp1 = 25;

unsigned long Time = 0;
unsigned long previous_time = 0;
unsigned long booking_one =0;
bool my_booking1 = false;
bool already_booked = false;
const int booking_time = 5000; // varighet for booking 
bool booked_status1 = false;
int Time_to_wait = 500;

const char* ssid = "Trym sin iPhone";
const char* password = "eplepai1";
const char* mqtt_server = "172.20.10.5";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
void booking_state(String topic, byte* message, unsigned int length){
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  if(topic=="homeHub/booking/bathroom1"){
    if(messageTemp == "booked"){
      booked_status1 = true;
      Serial.println(booked_status1);
    }
    else if(messageTemp == "available"){
      booked_status1 = false;
      Serial.println(booked_status1);
    }
  }
}
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.subscribe("homeHub/booking/bathroom1");
    }
  }
}
void setup(){
  pinMode(button, INPUT_PULLUP); 
  pinMode(lamp, OUTPUT); 
  pinMode(lamp1, OUTPUT); 
  Serial.begin(115200); 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(booking_state);
}
void loop(){ 
  Time = millis();
  reset_status();
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
  change();
  led();
  
}
void change(){  
  
  if ((digitalRead(button)==HIGH)&&(Time >= previous_time + Time_to_wait)){  
    if (booked_status1 == false){
      client.publish("homeHub/booking/bathroom", "booked");
      client.publish("homeHub/booking/ID", "Eirik");
      my_booking1 = true;
    }
    else{
      Serial.println("fully booked");
    }
  }
  while(digitalRead(button)==HIGH){
    previous_time = Time;
  }
}
void reset_status(){
  if ((my_booking1 == true) && (booked_status1 == false)&&(Time >= previous_time + Time_to_wait)){
    client.publish("homeHub/booking/ID", "Available");
    my_booking1 = false;
  }
}

void led(){
  if (booked_status1 == false){
    digitalWrite(lamp, HIGH);
    digitalWrite(lamp1, LOW);
  }
  else{
    digitalWrite(lamp, LOW);
    digitalWrite(lamp1, HIGH);
  }
}