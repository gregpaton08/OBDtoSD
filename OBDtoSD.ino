#include <Arduino.h>
#include <SD.h>

File OBDLog;
unsigned long startTime;

void setup() {
  Serial.begin(38400);
  
  // Set ss pin for SD Card
  pinMode(4, OUTPUT);
  if (false == SD.begin(4)) {
    return;
  }
  
  // determine file name
  char filename[20];
  for (uint16_t index = 1; index < 65535; index++) {
    sprintf(filename, "OBDLog%05d.txt", index);
    if (false == SD.exists(filename)) {
      break;
    }
  }
  sprintf(filename, "OBDLog.txt");
  OBDLog = SD.open(filename, FILE_WRITE);
  OBDLog.println("---START---");
  OBDLog.println("");
  OBDLog.println("");
   
  delay(1000);
  
  startTime = millis();
}


void loop() {
  Serial.write("01 0C 1\r\n");
  
  while (true) {
    //OBDLog.println("---LOOP BEGIN---");
    
    // wait until serial data is available
    while(Serial.available() == 0); 
    // seek to beginning of returned data
    while(false == Serial.find("4"));
    delay(200);
    OBDLog.print(millis() - startTime);
    OBDLog.print(", ");
    OBDLog.write('4');
    while (Serial.available() > 0) {
      unsigned char c = Serial.read();
      if (c == '\n' || c == '\r') break;
      OBDLog.write(c);
    }
    OBDLog.println("");
    //OBDLog.println("---LOOP END---");
    OBDLog.flush();
    break;
  }
}
