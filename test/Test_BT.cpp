#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

//Définition des pins
#define statePin 2
#define rxPin 3 //Pin Rx du HC05 --> Pin 3 de la Nano
#define txPin 4 //Pin Tx du HC05 --> Pin 4 de la Nano
#define keyPin 5

//Définition des baudrates des communications séries
#define hc05BR 38400
#define nanoBR 9600

bool attente=true;
int incomingByte;

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
SoftwareSerial hc05(txPin, rxPin); //Tx | Rx pdv HC05

void setup()
{
  Serial.begin(nanoBR);
  pinMode(keyPin, OUTPUT);

  //Mode échange de données
  digitalWrite(keyPin, LOW); attente=false;

  hc05.begin(hc05BR);

  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();// clear display
}

void loop()
{
  //Affichage des données sur le LCD
 if (hc05.available())  {lcd.setCursor(0, 0);  lcd.println(hc05.read());}
}

