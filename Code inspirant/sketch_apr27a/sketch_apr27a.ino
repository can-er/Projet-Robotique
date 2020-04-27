/* Code pour piloter des moteurs quand le capteur Ã  ultrasons dÃ©tecte un obstacle */
/* Robot Ã©viteur d'obstacles */

//Moteur A connectÃ© entre les pins A01 et A02 du Motor Driver TB6612FNG
//Moteur B connectÃ© entre les pins B01 et B02 du Motor Driver TB6612FNG

int STBY = 10; // standby Pin (si HIGH alors on peut faire tourner les moteurs, si LOW, les moteurs s'arrÃªtent)

//Motor A
int PWMA = 5; //Pin qui contrÃ´le la vitesse du moteur A (nombre entre 0 et 255 donc on doit le mettre sur un port PWM)
// AIN1 et AIN2 sont les pins de sens de rotation du moteur A
// Le moteur A tourne dans le sens horaire si le pin AIN1 est LOW et le pin AIN2 est HIGH
// Inversement, si AIN1 est HIGH et AIN2 est LOW, il tourne dans le sens antihoraire
// (si les deux sont LOW ou les deux sont HIGH, il ne tourne pas)
int AIN1 = 9; //Sens de rotation
int AIN2 = 8; //Sens

//Motor B
int PWMB = 6; //Pin qui contrÃ´le la vitesse du moteur B
// Pins de sens de rotation du moteur B
int BIN1 = 11; //Sens
int BIN2 = 12; //Sens

/* Constantes pour les broches */
const byte TRIGGER_PIN = 2; // Broche TRIGGER
const byte ECHO_PIN = 3;    // Broche ECHO
const byte LED_PIN = 7;  // Broche LED

/* Constantes pour le timeout */
const unsigned long MEASURE_TIMEOUT = 50000UL; // 25ms = ~8m a 340m/s

/* Vitesse du son dans l'air en mm/microsecondes */
const float SOUND_SPEED = 340.0 / 1000;


long randNumber;


/** Fonction setup() */
void setup() {
  /* Initialise le port serie */
  Serial.begin(115200);

  /* Capteur Ultrasons */
  // Initialise les broches pour le capteur Ã  ultrasons
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit etre LOW au repos
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);

  /* Moteurs */
  // On dÃ©finit tous les ports comme sorties pour piloter les moteurs
  pinMode(STBY, OUTPUT);
  // Commandes Moteur A
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  // Commandes Moteur B
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

}


void loop() {
  move(1, 100, 1); //motor 1, full speed, left
  move(0, 100, 1); //motor 2, full speed, left

  /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10 microsec sur la broche TRIGGER */
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son echo (s'il existe) */
  long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
  /* 3. Calcule la distance Ã  partir du temps mesurÃ© */
  float distance_mm = measure / 2.0 * SOUND_SPEED;

  if (distance_mm / 1000 < 0.2) {
    digitalWrite(LED_PIN, HIGH); // si un obstacle est Ã  moins de 30 cm
    stop(); // Stopper les moteurs
    delay(2000); //hold for 250ms until move again

    move(1, 50, 0); //motor 1, recule
    move(0, 50, 0); //motor 2, recule
    delay(3000); // recule pendant 2s
    stop(); // Stopper les moteurs
    delay(1000); //hold for 250ms until move again

    // On fait tourner le robot dans une direction alÃ©atoire
    randNumber = random(100.0);
    if (randNumber < 50.0) {
      move(1, 100, 1); //motor 1, tourne Ã  gauche
      move(0, 100, 0); //motor 2, tourne
    } else {
      move(1, 100, 0); //motor 1, tourne Ã  droite
      move(0, 100, 1); //motor 2, tourne
    }
    delay(1500); // tourne pendant 1s
    stop(); // Stopper les moteurs
    delay(1000); //hold for 250ms until move again

    digitalWrite(LED_PIN, LOW);
  }
  //  // Affiche les rÃ©sultats en m
  //  Serial.print(F("Distance: "));
  //  Serial.print(distance_mm / 1000.0, 2);
  //  Serial.println(F("m)"));


  /* DÃ©lai d'attente pour Ã©viter d'afficher trop de rÃ©sultats Ã  la seconde */
  delay(200);


}





void move(int motor, int speed, int direction) {
  // Function move(): Move specific motor at speed and direction
  //
  // Inputs:
  // motor: 0 for B and 1 for A
  // speed: number between 0 and 255: 0 is off, and 255 is full speed
  // direction: 0 clockwise, 1 counter-clockwise

  // DEMARRAGE
  digitalWrite(STBY, HIGH); //On met ce Pin sur HIGH avant de dÃ©marrer les moteurs

  // SENS DE ROTATION
  // Sens Horaire
  // Le moteur A tourne dans le sens horaire si le pin AIN1 est LOW et le pin AIN2 est HIGH (si les deux sont LOW ou les deux sont HIGH, il ne tourne pas)
  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;
  // Sens Anti-Horaire
  if (direction == 1) { // Si on change la direction dans le sens antihoraire
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  // ENVOI DES INSTRUCTIONS DE SENS DE ROTATION ET DE VITESSE AUX MOTEURS
  if (motor == 1) { // MOTEUR A
    digitalWrite(AIN1, inPin1);
    digitalWrite(AIN2, inPin2);
    analogWrite(PWMA, speed);
  } else { // if motor == 0 (MOTEUR B)
    digitalWrite(BIN1, inPin1);
    digitalWrite(BIN2, inPin2);
    analogWrite(PWMB, speed);
  }
}

void stop() {
  //Function stop(): Appeler cette fonction dans le programme pour stopper les moteurs
  digitalWrite(STBY, LOW);
}
