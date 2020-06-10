#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <L298NX2.h>
#include <IRremote.h>

#define LED_VERTE      18
#define LED_ROUGE     17
#define MOTEUR_DROIT   8
#define MOTEUR_GAUCHE  7
#define SS_PIN        53 // RFID (SPI)   
#define RST_PIN        9 // RFID (SPI)
#define TRIGGER_PIN    2 // Broche TRIGGER
#define ECHO_PIN       3
#define RECV_PIN      12 /* Du au mauvais fonctionnement de nos module Bluetooth, nous avons pris une commande infrarouge.*/

// constantes pour la calcul de la distance entre l'obstacle et le capteur HC-SR04
const unsigned long MEASURE_TIMEOUT = 50000UL; // 25ms = ~8m a 340m/s
const float SOUND_SPEED = 340.0 / 1000; //Vitesse du son dans l'air en mm/microsecondes 
int distance_cm;
int distance_obstacle = 50; // distance minimale entre le robot et l'obstacle

//const int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;


// initialisation du lecteur UID.
MFRC522 rfid(SS_PIN, RST_PIN);   

//initialisation du bus I2C
LiquidCrystal_I2C lcd(0x27,16,2);


String myKey;   // String retourne par le detecteur UID                
String cmd;


bool Starter_flag = false;       // variable permettant de passer en mode marche grâce à la carte magnetique
bool Turn_flag = false;         // Variable permettant d'entrer dans une boucle pour eviter les obstacles


// Fonciton pour lire l'UID de la carte magnetique

void Find_UID() {
 
  if ( ! rfid.PICC_IsNewCardPresent())    //detecter La carte, si il n'y en a pas on loop tant qu'aucune carte n'est presentee
    return;
  if ( ! rfid.PICC_ReadCardSerial())      // Si il y a une carte, on retourne son nom
    return;

     
  String myKey=String(rfid.uid.uidByte[0],HEX) + " " + String(rfid.uid.uidByte[1],HEX) + " " + String(rfid.uid.uidByte[2],HEX) + " " + String(rfid.uid.uidByte[3],HEX);
  delay(500);

  if ((myKey == "e0 58 45 1a" or "26 5d 5e 34") and (Starter_flag == 0)){   //Condition pour rentrer dans laboucle Starter_flag
  
  Starter_flag = 1;
  delay(100);

  }
  else if ((myKey == "e0 58 45 1a" or "26 5d 5e 34") and (Starter_flag == 1)){ //Condition pour sortir de la boucle Starter_flag
  
  
  Starter_flag = 0;
  }

}

void IR(){ // detection et affichage de la valeur lue envoyee et reçue par infrarouge
  if (irrecv.decode(&results)){
        cmd = String(results.value);
        Serial.println(cmd);
        irrecv.resume();
  }

}

void init_sequence(){

  Serial.begin(9600); //Vitesse de communication pour le port serie
  
  SPI.begin();
  rfid.PCD_Init();
  
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(MOTEUR_GAUCHE, OUTPUT);
  pinMode(MOTEUR_DROIT, OUTPUT);


  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit etre LOW au repos
  pinMode(ECHO_PIN, INPUT);

  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("----Show ID-----");

}

void wrong_UID_sequence(){
 
 
  lcd.setCursor(0,0);
  lcd.print("----Show ID-----");
  lcd.setCursor(4,1);
  lcd.print("Not ready");
  
  
  Find_UID();
  Serial.print(myKey);
  digitalWrite(LED_VERTE,LOW);
  digitalWrite(LED_ROUGE,HIGH);

  digitalWrite(MOTEUR_GAUCHE, LOW);
  digitalWrite(MOTEUR_DROIT, LOW);

}

void set_directions(){
  // on avance:
  if (cmd == "16718055"){ 
    digitalWrite(MOTEUR_GAUCHE, HIGH);
    digitalWrite(MOTEUR_DROIT, HIGH);      
  }
 // on s'arrete:
  if (cmd == "16726215" or cmd =="4294967295"){ 
    digitalWrite(MOTEUR_GAUCHE, LOW);
    digitalWrite(MOTEUR_DROIT, LOW);
  }
  
  // on tourne a droite:
   if (cmd == "16716015"){ 
    digitalWrite(MOTEUR_GAUCHE, HIGH);
    digitalWrite(MOTEUR_DROIT, LOW); 
  }

  // on tourne a gauche:
   if (cmd == "16734885"){ 
    digitalWrite(MOTEUR_GAUCHE, LOW);
    digitalWrite(MOTEUR_DROIT, HIGH);
  
  
  }  
}


void turn_to_left(){
  digitalWrite(MOTEUR_GAUCHE, LOW);
  digitalWrite(MOTEUR_DROIT, HIGH);
  Turn_flag = 0;
  
}



void setup(){
  init_sequence();
}

void loop(){

  while(!Starter_flag){
    wrong_UID_sequence();  
    Serial.println(Turn_flag);
  }

  while(Starter_flag and !Turn_flag){
    
    
    digitalWrite(LED_ROUGE, LOW);
    digitalWrite(LED_VERTE, HIGH);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("-----ID OK------");
    
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    //Mesure le temps entre l'envoi de l'impulsion ultrasonique et son echo (s'il existe)
    long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
    // Calcule la distance Ã  partir du temps mesure
    float distance_cm = measure / 20.0 * SOUND_SPEED;
    Serial.println(distance_cm);
    delay(200);
    
    Find_UID();
    IR();
    set_directions();    

    if (distance_cm < distance_obstacle){
    Turn_flag = 1;
     }
    Serial.println(Turn_flag); 

    
  }
    if(Turn_flag){

     turn_to_left(); 
      
    }
    
}  
   
