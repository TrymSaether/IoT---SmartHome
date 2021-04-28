#include <WiFi.h>
#include <PubSubClient.h>

const int button = 15; 
const int lamp = 2;

unsigned long Time = 0;
unsigned long previous_time = 0;

unsigned long booking_one =0;
unsigned long booking_two =0;
unsigned long booking_three =0;
bool my_booking1 = false;
bool my_booking2 = false;
bool my_booking3 = false;
bool already_booked = false;
const int booking_time = 1200000; // 20 min 
char msgtext[25];
char msgtext1[25];
bool booked_status1 = false;
bool booked_status2 = false;
bool booked_status3 = false;

const char* ssid = "ASUS_30_2G";
const char* password = "mimmi1918";
const char* mqtt_server = "192.168.1.11";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  // Connecting to a WiFi network
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


  if(topic=="home/booking1"){
      if(messageTemp == "booked"){
        booked_status1 = true;
      }
      else if(messageTemp == "available"){
        booked_status1 = false;
      }
  }

  if(topic=="home/booking2"){
      if(messageTemp == "booked"){
        booked_status2 = true;
      }
      else if(messageTemp == "available"){
        booked_status2 = false;
      }
  }
  if(topic=="home/booking3"){
      if(messageTemp == "booked"){
        booked_status3 = true;
      }
      else if(messageTemp == "available"){
        booked_status3 = false;
      }
  }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266Client")) {
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("home/booking1");
      client.subscribe("home/booking2");
      client.subscribe("home/booking3");
    }
  }
}
void setup(){
  pinMode(button, INPUT_PULLUP); 
  pinMode(lamp, OUTPUT); 
  Serial.begin(115200); 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(booking_state);
}

void loop(){ 
  Time = millis();
  
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
  
  change();
  reset_status();
  led();

}
void change(){  
  int Time_to_wait = 500;
  if ((digitalRead(button)==HIGH)&&(Time >= previous_time + Time_to_wait)){  
    if ((booked_status1 == false)&&(already_booked == false)){
      sprintf(msgtext,"Eirik");
      sprintf(msgtext1,"booked");
      client.publish("home/booking1", msgtext1);
      client.publish("Booking1", msgtext);
      booking_one = Time;
      my_booking1 = true;
      already_booked = true;
    }
    else if ((booked_status2 == false)&&(already_booked == false)){
      sprintf(msgtext,"Eirik");
      sprintf(msgtext1,"booked");
      client.publish("home/booking2", msgtext1);
      client.publish("Booking2", msgtext);
      booking_two = Time;
      my_booking2 = true;
      already_booked = true;
    }
    else if ((booked_status3 == false)&&(already_booked == false)){
      sprintf(msgtext,"Eirik");
      sprintf(msgtext1,"booked");
      client.publish("home/booking3", msgtext1);
      client.publish("Booking3", msgtext);
      booking_three = Time;
      my_booking3 = true;
      already_booked = true;
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
  if ((my_booking1 == true) && (booking_one + booking_time <= Time)){
    sprintf(msgtext1,"available");
    sprintf(msgtext,"Available");
    client.publish("Booking1", msgtext);
    client.publish("home/booking1", msgtext1);
    my_booking1 = false;
    already_booked = false;
  }
  else if ((my_booking2 == true) && (booking_two + booking_time <= Time)){
    sprintf(msgtext1,"available");
    sprintf(msgtext,"Available");
    client.publish("Booking2", msgtext);
    client.publish("home/booking2", msgtext1);
    my_booking2 = false;
    already_booked = false;
    }
  else if ((my_booking3 == true) && (booking_three + booking_time <= Time)){
    sprintf(msgtext1,"available");
    sprintf(msgtext,"Available");
    client.publish("Booking3", msgtext);
    client.publish("home/booking3", msgtext1);
    my_booking3 = false;
    already_booked = false;
  }
}
void led(){
  if (((booked_status1 == false)||(booked_status2 == false)||(booked_status3 == false))&&(already_booked == false)){
    digitalWrite(lamp, HIGH);
  }
  else{
    digitalWrite(lamp, LOW);
  }
}


