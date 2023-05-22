#include <Arduino.h>
#include <SoftwareSerial.h>

#define statePin 2
#define rxPin 3 //Pin Rx du HC05 --> Pin 3 de l'Arduino
#define txPin 4//Pin Tx du HC05 --> Pin 4 de l'Arduino
#define keyPin 5
#define hc05BR 38400
#define nanoBR 9600

int incomingByte;
SoftwareSerial hc05(txPin, rxPin); //Tx | Rx pdv HC05
//Projet 1A: 98d3:11:fcc42f
//Panier: 2016,11,298784
//Robot: 98D3:41:F62269

void setup()
{
  Serial.begin(nanoBR);
  pinMode(keyPin, OUTPUT);  digitalWrite(keyPin, HIGH); // Mode AT  
  Serial.println("Enter AT commands:");
  hc05.begin(hc05BR);
}

void loop()
{
 if (hc05.available()) Serial.write(hc05.read());
 if (Serial.available()) {incomingByte=Serial.read(); Serial.write(incomingByte); hc05.write(incomingByte);}
}
