#define type 0

// Bibliotek
#include <Arduino.h>
/*
- Ringeknapp (angir beboer som vedkommende skal besøke): // knappPin, knappState
    Knappen trykkes og holdes i minst 3 sekunder, if(knappState == 1 og millis>3 sek)
    og deretter trykkes knappen det antall ganger som tilsvarer beboer- nummeret. beboer array[5], 
Riktig beboer skal så få lyd og lyssignal på sin ESP32-ctrl, 
og må gi et signal (trykknapp) på at det er hans/hennes besøkende.
- Respons på ringeknapp: 
det sjekkes om maks antall personer i kollektivet er nådd, 
og hvis ja så må den beboer som har den først ankomne besøkende be vedkommende om å gå (trykknapp), 
men bare hvis den besøkende har vært der minst 30 minutter. 
Har den besøkende vært der mindre enn 30 minutter,
får den som ringer på "rødt lys" 
og beskjed om antall minutter før han/hun kan prøve igjen.
- Ringeknapp(ingen angivelse av hvem av beboerne som vedkommende ønsker å treffe): 
det gis lyd/lyssignal på alle konsollene i kollektivet. 
Vi antar at en av beboerne går til inngangsdøren og sjekker. 
Deretter må én av beboerne gi signal (trykknapp) på at det er hans/hennes besøkende, 
eller så trykker den som gikk til døra 2 ganger på knappen på sitt konsoll,
og det betyr at det f.eks. bare var noen som spurte etter en gateadresse el. lign. 
Ringeren nulles ut av systemet. */
// Variabler

const int displayPins[7] = {25,33,31,13,2,4,5}; // {G, F, A, B, E, D, C}
const int decimalPin = 21; // decimalpoint
const int buttonPin = 12;
int returnValue = 0;
// 7 Segment array med bytes
byte displayLEDS[10][7] = 
 {{0,1,1,1,1,1,1},  //0
  {0,0,0,1,0,0,1},  //1
  {1,0,1,1,1,1,0},  //2
  {1,0,1,1,0,1,1},  //3
  {1,1,0,1,0,0,1},  //4
  {1,1,1,0,0,1,1},  //5
  {1,1,1,0,1,1,1},  //6
  {0,0,1,1,0,0,1},  //7
  {1,1,1,1,1,1,1},  //8
  {1,1,1,1,0,1,1}}; //9
// Funksjoner




// Skriver et tall på 7 segment
void writeDigit(int digit){
  for(int i=1; i<=6; i++){
    digitalWrite(displayPins[i], convertHighLow(displayLEDS[digit][i]));
  }
}
// Konverterer fra høy til lav (0,1)
int convertHighLow(int v){
  int convertedValue;
  if(type == 0){
    convertedValue = 1 - v;
  }
  else{
    convertedValue = 0;
  }
  return(convertedValue);
}

float resetTime(float& time){
  time = (time - millis())/1000;
  return(time);
}
// Sjekker
bool ringeklokke(bool& buttonState, int& time){
  int time1 = resetTime(time);
  if(buttonState != 0 && time>=3){
    return(true); 
  }
  else{
    return(false);
  }
}

int gjest_besok(int& buttonState, int& time){
  int time2 = resetTime(time);
  if(time2>10){
    return(0);
  }
  else{
    return(1)
  }
}

int teller(int& buttonState, int teller_besok){
  while(buttonState == 1){
    teller_besok += 1;
    writeDigit(teller_besok);
    if(teller_besok >6){
      teller_besøk = 0;
    }
  }
}

struct beboere
{
  ALEX = 1,
  LOURENZO = 2,
  ISAK = 3,
  TRYM = 4,
  EIRIK = 5,
  KRISTIAN = 6
};


void setup() {
  pinMode(buttonPin, OUTPUT);
  Serial.begin(115200);
}

void loop(){
  float time = millis();
  bool buttonState = digitalRead(buttonPin);
  bool dingdong = ringeklokke(buttonState);

}
