#define uS_TO_S_FACTOR 1000000*60*60//conversion for mikroseconds to seconds to minutes to hours
#define TIME_TO_SLEEP 8  //8 timer søvn

RTC_DATA_ATTR int bootCount =0;


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  
  wakeup_reason= esp_sleep_get_wakeup_cause();
  
switch(wakeup_reason)
{
  case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
  case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
  case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
  case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
  default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }

}



void setup() {
  Serial.begin(115200);
  delay(1000);

++bootCount;
Serial.println("Boot number: "+ String(bootCount));

print_wakeup_reason(); //optional

esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP *uS_TO_S_FACTOR);
Serial.println("setup ESP32 to sleep for every " + String(TIME_TO_SLEEP)+"Seconds");


Serial.println("Going to sleep now");
delay(100);
Serial.flush();
esp_deep_sleep_start();
Serial.println("This will never be printed");
}
void loop() {
  // put your main code here, to run repeatedly:

}
