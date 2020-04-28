char[25];

void setup()
{
  Serial.begin(9600); //set baud rate
  pinMode(2, OUTPUT);
}

void loop(){
  while(Serial.available())
  {
    message+=char(Serial.read());
  }
  if(!Serial.available())
  {
    if(message != "") // if we received some data, then light up an LED
    {      
      Serial.println(message);
      message="";
    }
  }
  delay(200);
}
   
