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
   
  // wait a second
  delay(1000);
  // okay
  
  startTime = millis();
}


void loop() {
  // RPM
  //logPID("01 0C");
  logPID(0x01, 0x0C);
  // Speed
  //logPID("01 0D");
  logPID(0x01, 0x0D);
  // MAF
  //logPID("01 10");
  logPID(0x01, 0x10);
  // Throttle Position
  //logPID("01 11");
  logPID(0x01, 0x11);
}


void logPID(char *PID) {
  Serial.write(PID);
  Serial.write(" 1\r\n");  
  
  while (true) {
    // wait until serial data is available
    while(Serial.available() == 0); 
    // seek to beginning of returned data
    while(false == Serial.find("4"));
    // print time and '4' that was skipped by Serial.find()
    OBDLog.print(millis() - startTime);
    OBDLog.print(", ");
    OBDLog.write('4');
    // loop until new line character found
    while (true) {
      unsigned char c = Serial.read();
      if (c == (unsigned char)-1) continue;
      if (c == '\n' || c == '\r') break;
      OBDLog.write(c);
    }
    OBDLog.println("");
    // flush data to SD card
    OBDLog.flush();
    break;
  }
}


void logPID(byte mode, byte pid) {
  // Query PID
  switch (mode) {
    case 0x01:
      switch (pid) {
        // RPM
        case 0x0C:
          Serial.write("01 0C 1\r\n"); 
          break;
        // Speed
        case 0x0D:
          Serial.write("01 0D 1\r\n"); 
          break;
        // MAF
        case 0x10:
          Serial.write("01 10 1\r\n"); 
          break;
        // Throttle Position
        case 0x11:
          Serial.write("01 11 1\r\n"); 
          break;
        default:
          return;
      }
      break;
    default:
      return;
  }
  
  
  while (true) {
    // wait until serial data is available
    while(Serial.available() == 0); 
    // seek to beginning of returned data
    while(false == Serial.find("4"));
    // print time and '4' that was skipped by Serial.find()
    OBDLog.print(millis() - startTime);
    OBDLog.print(", ");
    OBDLog.write('4');
    // loop until new line character found
    while (true) {
      unsigned char c = Serial.read();
      if (c == (unsigned char)-1) continue;
      if (c == '\n' || c == '\r') break;
      OBDLog.write(c);
    }
    OBDLog.println("");
    // flush data to SD card
    OBDLog.flush();
    break;
  }
}
