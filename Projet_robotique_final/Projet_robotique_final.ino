#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <L298NX2.h>
#include <IRremote.h>


// constante pour la calcul de la distance entre l'obstacle et le capteur HC-SR04

const byte TRIGGER_PIN = 2; // Broche TRIGGER
const byte ECHO_PIN = 3;    // Broche ECHO
const unsigned long MEASURE_TIMEOUT = 50000UL; // 25ms = ~8m a 340m/s
const float SOUND_SPEED = 340.0 / 1000; /* Vitesse du son dans l'air en mm/microsecondes */
int distance_cm;


// Du au mauvais fonctionnement de nos module Bluetooth, nous avons pris une commande infrarouge.
const int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;

// initialisation des pins du lecteur RFID
#define SS_PIN 53     
#define RST_PIN 9

// initialisation du lecteur UID.
MFRC522 rfid(SS_PIN, RST_PIN);   

//initialisation du bus I2C
LiquidCrystal_I2C lcd(0x27,16,2);


String myKey;   // String retourné par le detecteur UID                
String cmd;


bool Starter_flag = false;       // variable permettant de passer en mode marche grâce à la carte magnétique
bool Turn_flag = false;         // Variable permettant d'entrer dans une boucle pour éviter les obstacles


// Fonciton pour lire l'UID de la carte magnétique

void Find_UID() {
 
  if ( ! rfid.PICC_IsNewCardPresent())    //detecter La carte, si il n'y en a pas on loop tant qu'aucune carte n'es présentée
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

void IR(){ // détection et affichage de la valeur lue envoyée et reçue par infrarouge
  if (irrecv.decode(&results)){
        cmd = String(results.value);
        Serial.println(cmd);
        irrecv.resume();
  }

}



void setup(){

 
  Serial.begin(9600); //Vitesse de communication pour le port série
  
  SPI.begin();
  rfid.PCD_Init();
  
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(18, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(42, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit etre LOW au repos
  pinMode(ECHO_PIN, INPUT);

  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Show ID...");

}

void loop(){

  while(!Starter_flag){
    
  
  lcd.setCursor(0,0);
  lcd.print("Show ID");

  lcd.setCursor(0,1);
  lcd.print("Not ready");
  
  
  Find_UID();
  Serial.print(myKey);
  digitalWrite(18,LOW);
  digitalWrite(17,HIGH);

  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  
  }

  while(Starter_flag and !Turn_flag){
    
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ID OK");
    
    
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son echo (s'il existe) */
    long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
    /* 3. Calcule la distance Ã  partir du temps mesurÃ© */
    float distance_cm = measure / 20.0 * SOUND_SPEED;
   
    
    digitalWrite(18, HIGH);
    digitalWrite(17,LOW);
    Find_UID();
    IR();
    

    if (cmd == "16718055"){
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);      
    }

      
    if (cmd == "16726215" or cmd =="4294967295"){
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
    }
    
     if (cmd == "16716015"){
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW); 
    }

    
     if (cmd == "16734885"){
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
    }
    
    if (distance_cm < 50){
      Turn_flag = 1;
    }

       
  }
    while (Turn_flag){

      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      
      Turn_flag = 0;
      delay(300);
      
    }
    
}  
   
   

    
   
    
  

  
    
  
