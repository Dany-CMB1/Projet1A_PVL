#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

//Déclaration pins module BT
#define statePin 2
#define rxPin 3 //Pin Rx du HC05 --> Pin 3 de l'Arduino
#define txPin 4//Pin Tx du HC05 --> Pin 4 de l'Arduino
#define keyPin 5

//Définition des macros: on remplace les msgs texte par des entiers dans les protocoles de communication
#define CALIBRAGE_TEMPS 5
#define CALIBRAGE_POT 6
#define TOP_DEPART 7

//Déclaration des pins sur lesquels les capteurs sont branchés
int GLED = 4;
int Laser = 2;
int LightSensor= 0;
int Pot = 5;

int directionPin = 12;
int pwmPin = 3;
int brakePin = 9;

//uncomment if using channel B, and remove above definitions
//int directionPin = 13;
//int pwmPin = 11;
//int brakePin = 8;

//boolean to switch direction
bool directionState;

//Initialisation des variables
bool currStatus;
bool prevStatus;
bool chrono = 0;
int nb_fd, nb_fm;
int seuil = 0, pot = 0;
double tps, start;
unsigned long time_offset; //Décalage temporel (millis()) de la Uno par rapport à la nano

SoftwareSerial hc05(txPin, rxPin); //Tx | Rx pdv HC05
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

void setup()
{
  // Init Serial at 115200 Baud Rate 
  Serial.begin(115200); 

  pinMode(directionPin, OUTPUT); digitalWrite(directionPin,HIGH);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT); digitalWrite(brakePin, LOW);

  //Déclaration de l'état des capteurs digitaux (entrée ou sortie)
  pinMode(Laser, OUTPUT); digitalWrite(Laser, HIGH); 
  
  //Initialisation LED de fonctionnement
  pinMode(GLED, OUTPUT); 
  digitalWrite(GLED, LOW); 

  //Initialisation de l'écran LCD 
  lcd.init(); 
  lcd.backlight();
  lcd.clear();   

  //Calibrage temps et potard
  calibratePot();
  calibrateTime();

  digitalWrite(pwmPin, 100);    

}


void loop()
{
  //Acquisition des états logiques des BO (1: passant, 0: bloqué)
  GetSensorStatus();

  //Màj du nb de fm et fd
  //Détection des fd
  if (!currStatus && prevStatus) {nb_fd++;}

  //Détection des fm
  if (currStatus  && !prevStatus ) {nb_fm ++;}

  //Etat de base: attente du départ
  if (!chrono) Serial.println("Attente départ");

  //Mesure de l'écoulement du temps:
  if (chrono) {tps=(time_offset + millis()-start)/(double)1000; Serial.println(tps); }

  //Détection du 1er front montant de la BO de départ; démarrage du chrono
  else if (hc05.available() && hc05.read()==2 && !chrono) {chrono = 1; start = time_offset + millis(); digitalWrite(GLED, HIGH);}

  //Détection du 1er front descendant de la BO d'arrivée; arrêt du chrono, affichage continu du temps final
  if (nb_fd==1 && chrono) {chrono = 0; digitalWrite(GLED, LOW); digitalWrite(pwmPin,0); LCDprintTime();}

}

void LCDprintTime()
{
  lcd.setCursor(0,0);
  lcd.print(tps);
}

void GetSensorStatus()
{
  prevStatus = currStatus;
  //Lecture des valeurs analogiques des photodiodes
  //Conversion analogique --> logique
  if (analogRead(LightSensor)>seuil) currStatus =1; 
  else currStatus =0; 
}


void calibrateTime()
{
  //Attente synchronisation demande calibrage temps
  while(!(hc05.available() && hc05.read()==CALIBRAGE_TEMPS)){hc05.write(CALIBRAGE_TEMPS);}
  
  //Attente réponse 
  while(!hc05.available()){}

  //Lecture de la valeur millis() de la nano
  for (int i=0;i<sizeof(int); i++) {if (hc05.available()) time_offset += hc05.read()<<(sizeof(int)*i);}
  time_offset -= millis();
 }

 void calibratePot()
 {
  //Attente synchronisation demande régalge potentiomètre
  while(!(hc05.available() && hc05.read()==CALIBRAGE_POT)){hc05.print(CALIBRAGE_POT);}

  //Réglage du seuil en fonction du potentiomètre
  pot = 1023 - analogRead(Pot); 
  seuil = map(pot, 0, 1023, 0, 1000); 
  hc05.write(seuil);
 }