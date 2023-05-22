#include <Arduino.h>
#define NB_BO 2
#include <SoftwareSerial.h>

//Déclaration pins module BT
#define statePin 2
#define rxPin 3 //Pin Rx du HC05 --> Pin 3 de l'Arduino
#define txPin 4//Pin Tx du HC05 --> Pin 4 de l'Arduino
#define keyPin 5

//Déclaration des pins sur lesquels les capteurs sont branchés
int GLED = 4;
int Laser[NB_BO] = {2,3};
int LightSensor[NB_BO] = {0,1};
int Pot = 5;

//Initialisation des variables
bool currStatus[NB_BO];
bool prevStatus[NB_BO];
bool chrono = 0;
int nb_fd[NB_BO], nb_fm[NB_BO];
int seuil = 0, pot = 0;
double tps, start;

SoftwareSerial hc05(txPin, rxPin); //Tx | Rx pdv HC05

void setup()
{
  // Init Serial at 115200 Baud Rate 
  Serial.begin(115200); 
  //Déclaration de l'état des capteurs digitaux (entrée ou sortie)
  for (int i=0; i<NB_BO; i++) {pinMode(Laser[i], OUTPUT); digitalWrite(Laser[i], HIGH); }
  
  pinMode(GLED, OUTPUT); 
  digitalWrite(GLED, LOW); 
}


void loop()
{
  //Acquisition des états logiques des BO (1: passant, 0: bloqué)
  GetSensorStatus();

  //Màj du nb de fm et fd
  for (int i=0; i<NB_BO; i++) 
  {
    //Détection des fd
    if (!currStatus[i] && prevStatus[i]) {nb_fd[i]++;}

    //Détection des fm
    if (currStatus[i] && !prevStatus[i]) {nb_fm[i]++;}
  }  

  //Etat de base: attente du départ
  if (!chrono) Serial.println("Attente départ");

  //Mesure de l'écoulement du temps:
  if (chrono) {tps=(millis()-start)/(double)1000; Serial.println(tps); }

  //Détection du 1er front montant de la BO de départ; démarrage du chrono
  else if (hc05.available() && !chrono) {chrono = 1; start=millis(); digitalWrite(GLED, HIGH);}

  //Détection du 1er front descendant de la BO d'arrivée; arrêt du chrono, affichage continu du temps final
  if (nb_fd[NB_BO-1]==1 && chrono) {chrono = 0; digitalWrite(GLED, LOW);}

}



void GetSensorStatus()
{
  //Réglage du seuil en fonction du potentiomètre
  pot = 1023 - analogRead(Pot); 
  seuil = map(pot, 0, 1023, 0, 1000); 

  for (int i=0; i<NB_BO; i++) 
  {
    prevStatus[i] = currStatus[i];
    //Lecture des valeurs analogiques des photodiodes
    //Conversion analogique --> logique
    if (analogRead(LightSensor[i])>seuil) {currStatus[i]=1; }
    else {currStatus[i]=0; }
  }
}

void tempSync()
 {
  while (hc05.available()){
     time = hc05.readBytesUntil("f");
 }