/*
L293D *** Arduino
Pin 1 *** Pin 11
Pin 2 *** Pin 12
Pin 3 *** Moteur 1
Pin 4 *** GND
Pin 5 *** GND
Pin 6 *** Moteur 1
Pin 7 *** Pin 13
Pin 8 *** Pin Vin (9V) fonctionne si vous branchez une pile 9V sur l’arduino au lieu du connecteur USB.
Pin 9 *** Pin 9
Pin 10 *** Pin 7
Pin 11 *** Moteur 2
Pin 12 *** GND
Pin 13 *** GND
Pin 14 *** Moteur 2
Pin 15 *** Pin 8
Pin 16 *** Pin (5V)

Joystik *** Arduino
Pin Key *** Pin 2 (numérique côté droite de l’Arduino)
Pin X *** Pin A0
Pin Y *** Pin A1
Pin VCC *** Pin (5V)
Pin GND *** Pin (GND
*/

//Joystik
const int JOYvertical = 1; // analogique donc A1
const int JOYhorizontal = 2; // analogique donc A2
const int JOYselect = 2; // digital 2
int joyValue = 0;
int joyValueMax = 1023;
int joyValueMin = 0;
int joyValueMid = 512;
int joyValueMidUpper = joyValueMid + 20;
int joyValueMidLower = joyValueMid - 20;

//MOTEURS
int motor1_enablePin = 11; //pwm
int motor1_in1Pin = 13;
int motor1_in2Pin = 12;
int motor1Speed = 0;
int motor1SpeedMin = 100;
int motor1SpeedMax = 255;

int motor2_enablePin = 10; //pwm
int motor2_in1Pin = 8;
int motor2_in2Pin = 7;
int motor2Speed = 0;
int motor2SpeedMin = 100;
int motor2SpeedMax = 255;

void setup()
{
  //Joystik lecture
  pinMode(JOYvertical,INPUT);
  pinMode(JOYhorizontal,INPUT);
  pinMode(JOYselect,INPUT);
  digitalWrite(JOYselect,HIGH); // pull-up
  Serial.begin(9600);

  //MOTEURS
  //on initialise les pins du moteur 1
  pinMode(motor1_in1Pin, OUTPUT);
  pinMode(motor1_in2Pin, OUTPUT);
  pinMode(motor1_enablePin, OUTPUT);
  //on initialise les pins du moteur 2
  pinMode(motor2_in1Pin, OUTPUT);
  pinMode(motor2_in2Pin, OUTPUT);
  pinMode(motor2_enablePin, OUTPUT);
}
void loop()
{
  //Joystik
  int AvAr, GaDr, select;
  // lecture des valeurs du joystick
  AvAr = analogRead(JOYvertical);    // entre 0 et 1023
  GaDr = analogRead(JOYhorizontal); // entre 0 et 1023
  select = digitalRead(JOYselect);      // HIGH (1)si non appuye, LOW (0)sinon

//MOTEURS 1
    joyValue = analogRead(JOYvertical);
 
    if(joyValue > joyValueMidUpper) //Forward
    {
        motor1Speed = map(joyValue, joyValueMidUpper, joyValueMax, motor1SpeedMin, motor1SpeedMax);
        Motor1Forward(motor1Speed);
    }
    else if(joyValue < joyValueMidLower) //Backward
    {
        motor1Speed = map(joyValue, joyValueMidLower, joyValueMin, motor1SpeedMin, motor1SpeedMax);
        Motor1Backward(motor1Speed);
    }
    //joyValue Between joyValueMidLower - joyValueMidUpper.
    //Need some range here, because joystick sometime not in  exact center.
    else
    {
       Motor1Stop();
    }

     //MOTEURS 2
    joyValue = analogRead(JOYhorizontal);
 
    if(joyValue > joyValueMidUpper) //Forward
    {
        motor2Speed = map(joyValue, joyValueMidUpper, joyValueMax, motor2SpeedMin, motor2SpeedMax);
        Motor2Forward(motor2Speed);
    }
    else if(joyValue < joyValueMidLower) //Backward
    {
        motor2Speed = map(joyValue, joyValueMidLower, joyValueMin, motor2SpeedMin, motor2SpeedMax);
        Motor2Backward(motor2Speed);
    }
    //joyValue Between joyValueMidLower - joyValueMidUpper.
    //Need some range here, because joystick sometime not in  exact center.
    else
    {
       Motor2Stop();
    }
}

void Motor1Forward( byte Spd)
{
    digitalWrite(motor1_in1Pin, HIGH);
    digitalWrite(motor1_in2Pin, LOW);
    analogWrite(motor1_enablePin, Spd);
}
void Motor1Backward( byte Spd)
{
    digitalWrite(motor1_in1Pin, LOW);
    digitalWrite(motor1_in2Pin, HIGH);
    analogWrite(motor1_enablePin, Spd);
}
void Motor1Stop()
{
    analogWrite(motor1_enablePin, 0);
}

void Motor2Forward( byte Spd)
{
    digitalWrite(motor2_in1Pin, HIGH);
    digitalWrite(motor2_in2Pin, LOW);
    analogWrite(motor2_enablePin, Spd);
}
void Motor2Backward( byte Spd)
{
    digitalWrite(motor2_in1Pin, LOW);
    digitalWrite(motor2_in2Pin, HIGH);
    analogWrite(motor2_enablePin, Spd);
}
void Motor2Stop()
{
    analogWrite(motor2_enablePin, 0);
}