#include <Arduino.h>
#include <SoftwareSerial.h>

//Définition des pins
#define statePin 2
#define rxPin 3 //Pin Rx du HC05 --> Pin 3 de la Nano
#define txPin 4 //Pin Tx du HC05 --> Pin 4 de la Nano
#define keyPin 5

//Définition des baudrates des communications séries
#define hc05BR 38400
#define nanoBR 9600

bool attente=true;
String choice;
int incomingByte;

SoftwareSerial hc05(txPin, rxPin); //Tx | Rx pdv HC05

void setup()
{
  Serial.begin(nanoBR);
  pinMode(keyPin, OUTPUT);

  //Choix du mode de fonctionnement
  while (attente)
  {
    Serial.print("Choose mode (at/data): ");
    while (Serial.available() ==0) {} //Boucle d'attente d'une entrée sur le moniteur série
    choice = Serial.readString(); choice.trim(); Serial.println(choice);
    if (choice=="at") {digitalWrite(keyPin, HIGH); Serial.println("Enter AT commands:"); attente=false;}
    else if (choice=="data") {digitalWrite(keyPin, LOW); attente=false;}
  }

  hc05.begin(hc05BR);
}

void loop()
{
 if (hc05.available()) Serial.write(hc05.read());
 if (Serial.available()) {incomingByte=Serial.read(); Serial.write(incomingByte); hc05.write(incomingByte);}
}

