#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX
String incoming = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  while (!Serial){
    ; // Wait for serial port to connect. Needed for native USB Only
  }

  mySerial.begin(38400);
  mySerial.println("Hello,world?");
}

void loop() {
  
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
    incoming = Serial.readString();

  if (incoming.equalsIgnoreCase("LON")){
    digitalWrite(13, HIGH);
  }
    

  if (incoming.equalsIgnoreCase("LOFF")){
    digitalWrite(13, LOW);
  }
  Serial.println("Incoming message:" + incoming);
  //delay(1000);

}
