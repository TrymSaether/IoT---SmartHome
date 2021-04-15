const int buttonPin = 2;     

int buttonState = 0;         
int counter = 0;
int prestate = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  Serial.println(counter);
  if (buttonState == HIGH && prestate == 0){
      counter = 1;
      prestate = 1;
      delay(1000);
  }
  else if (buttonState == HIGH && prestate == 1){
    counter = 0;
    prestate = 0;
    delay(1000);
    }
}