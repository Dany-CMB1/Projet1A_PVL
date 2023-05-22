#include <Arduino.h>
//Déclaration des pins sur lesquels les capteurs sont branchés
int GLED = 4;
int Laser = 2;
int LightSensor=0;
int Pot = 5;

//Initialisation des variables
int val_brute; 
int seuil = 0, pot = 0;

void setup() {
  // Init Serial at 115200 Baud Rate 
  Serial.begin(115200); 
  //Déclaration de l'état des capteurs digitaux (entrée ou sortie)
  pinMode(Laser, OUTPUT); pinMode(GLED, OUTPUT); 
  digitalWrite(GLED, LOW);
}

void loop() {
  digitalWrite(Laser, HIGH);
  val_brute = analogRead(LightSensor);

  //Réglage du seuil en fonction du potentiomètre
  pot = 1023 - analogRead(Pot); 
  seuil = map(pot, 0, 1023, 0, 1000); 

  if (val_brute>seuil) {Serial.println(1); digitalWrite(GLED, HIGH); }
  else {Serial.println(0); digitalWrite(GLED, LOW); }
}
