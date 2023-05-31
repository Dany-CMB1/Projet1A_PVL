#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

//Déclaration pins module BT
#define statePin 2
#define rxPin 3 //Pin Rx du HC05 --> Pin 3 de l'Arduino
#define txPin 7//Pin Tx du HC05 --> Pin 4 de l'Arduino
#define keyPin 8

#define CALIBRAGE_TEMPS 5
#define CALIBRAGE_POT 6
#define TOP_DEPART 7
#define FIN_COURSE 8
#define hc05BR 38400
#define nanoBR 9600

int directionPin = 12;
int pwmPin = 3;
int brakePin = 9;
//Déclaration des pins sur lesquels les capteurs sont branchés
int GLED = 4;
int Laser = 6;
int LightSensor = 2;
int Pot = 5;
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
//Initialisation des variables
bool currStatus;
bool prevStatus;
int val_brute; 
bool chrono = 0;
int nb_fd=0, nb_fm=0;
int seuil = 600, pot = 0;
double tps, start;

unsigned long millis_nano;
SoftwareSerial hc05(txPin, rxPin); //Tx | Rx pdv HC05
bool directionState;

void setup()
{ digitalWrite(pwmPin, LOW);
  digitalWrite(brakePin, HIGH);
  //Initialisation de l'écran LCD 
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();                 // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("Depart course");
  // Init Serial at 115200 Baud Rate 
  Serial.begin(9600); 
  hc05.begin(hc05BR);
  //Déclaration de l'état des capteurs digitaux (entrée ou sortie)
  pinMode(Laser, OUTPUT); 
  digitalWrite(Laser, HIGH); 
  
  pinMode(GLED, OUTPUT); 
  digitalWrite(GLED, LOW); 
  //calibratePot();
  lcd.clear();                 // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("Seuil :");
  lcd.setCursor(0, 8);
  lcd.print(seuil);

  pinMode(directionPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT);
  delay(2000);
  digitalWrite(pwmPin, HIGH);
  digitalWrite(brakePin, LOW);
}


void loop()
{
  //Serial.println(seuil);
  //Acquisition des états logiques des BO (1: passant, 0: bloqué)
  GetSensorStatus();

  //Détection des fd
  if (!currStatus && prevStatus) {nb_fd++;}

  //Détection des fm
  else if (currStatus && !prevStatus) {nb_fm++;}

  if(hc05.available() && hc05.read()==TOP_DEPART){
    chrono=1;
    digitalWrite(GLED, HIGH);
    start=(millis());
    nb_fd=0;
    }

  if (chrono) {
    //Serial.println(tps);
    tps=(millis()-start)/(double)1000; 
    lcd.clear();                 // clear display
    lcd.setCursor(0, 0);         // move cursor to   (0, 0)
    lcd.print(tps); }
    
  if (nb_fd>=1 && chrono) {
    digitalWrite(pwmPin, LOW);
    digitalWrite(brakePin, HIGH);
    chrono = 0; 
    digitalWrite(GLED, LOW);
    lcd.clear();                 // clear display
    lcd.setCursor(0, 0);         // move cursor to   (0, 0)
    lcd.print(tps);
    nb_fd=0;
    nb_fm=0;
    hc05.write(FIN_COURSE);}
  //Serial.println(nb_fd);
}



void GetSensorStatus()
{
  prevStatus = currStatus;
  //Lecture des valeurs analogiques des photodiodes
  //Conversion analogique --> logique
  if (analogRead(LightSensor)>seuil) currStatus=1; 
  else currStatus=0; 
  Serial.println(analogRead(LightSensor));
}

void calibratePot()
 {
  pot = 1023 - analogRead(Pot);
  seuil = map(pot, 0, 1023, 0, 1000);
  String seuilStr = String(seuil);
  hc05.println(seuilStr);
 }
