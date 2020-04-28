#include <SoftwareSerial.h> 

SoftwareSerial MyMaster(2, 3); // RX | TX 

int VRx = A0;
int VRy = A1;
int SW = 2;

int initxPosition;
int inityPosition;
int xPosition;
int yPosition;

bool moveFlag = false;

int SW_state = 0;
int mapX = 0;
int mapY = 0;

void debug_joystick(){
  Serial.print("X: ");
  Serial.print(mapX);
  Serial.print(" | Y: ");
  Serial.print(mapY);
  Serial.print(" | Button: ");
  Serial.println(SW_state);
  delay(100);
  }


void setup() {
  Serial.begin(9600); 
  initxPosition = map(analogRead(VRx), 0, 1023, -255, 255);
  inityPosition = map(analogRead(VRy), 0, 1023, -255, 255);
  
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);    
  
  MyMaster.begin(38400); 
}

void loop() {
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPosition, 0, 1023, -255, 255);
  mapY = map(yPosition, 0, 1023, -255, 255);
  delay(100);

  if ((mapX != initxPosition) || (mapY != inityPosition)){
    moveFlag = true;
    }
  //debug_joystick();
  else if ((mapX == initxPosition) && (mapY == inityPosition)){
    moveFlag = false;
    }
  if (moveFlag){
    Serial.println("Move");
    debug_joystick();
    MyMaster.write(mapX);
    MyMaster.write(mapY);
  	//MyMaster.println("test");
  	moveFlag = false;
    }
  else{
  	Serial.println("No Move")
  }
}
