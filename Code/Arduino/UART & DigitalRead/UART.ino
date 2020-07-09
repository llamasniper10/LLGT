//String rxString = ""; // for incoming serial data
//unsigned long previousMillis = 0;   
//const long interval = 3000;
//
//void setup() {
//  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
//}
//
//void loop() {
//
//  unsigned long currentMillis = millis();
//  
//  // send data only when you receive data:
//  if (Serial.available() > 0) {
//    // read the incoming byte:
//     rxString = Serial.readString();
//
//     Serial.print("I received: " + rxString);
//}
///*
//  if (currentMillis - previousMillis >= interval) {
//    // save the last time we wrote to the buffer
//    previousMillis = currentMillis;
//
//    Serial.write("Testing!!\n");
//  }
//  */
//}
 
int val = 0;  // variable to store the value read

void setup() {
  Serial.begin(9600);
  pinMode(7,INPUT);
}

void loop() {
  val = digitalRead(7);

  if (val == 1)
    Serial.println("HIGH");          // debug value
  else
    Serial.println("LOW");          // debug value
    
  delay(100);
}
