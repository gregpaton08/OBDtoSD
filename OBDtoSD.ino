#include <Arduino.h>
#include <SD.h>

File OBDLog;
unsigned long startTime;

#define SD_SS_PIN 4


void setup() {
  Serial.begin(38400);
  
  // Set ss pin for SD Card
  pinMode(SD_SS_PIN, OUTPUT);
  if (false == SD.begin(SD_SS_PIN)) return;
 
  // determine file name
  // file name must be 8 characters or less
  char filename[20];
  for (uint16_t index = 1; index < 65535; index++) {
    sprintf(filename, "OBD%05d.csv", index);
    if (false == SD.exists(filename)) {
      break;
    }
  }
  
  OBDLog = SD.open(filename, FILE_WRITE);
  OBDLog.println("---START---");
  OBDLog.println("");
  OBDLog.println("");
  
  Serial.println("DONE");
   
  // wait a second
  delay(1000);
  // okay
  
  startTime = millis();
}


void loop() {
  return;
  // RPM
  logPID(0x01, 0x0C);
  // Speed
  logPID(0x01, 0x0D);
  // MAF
  logPID(0x01, 0x10);
  // Fuel Level
  logPID(0x01, 0x2F);
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
  // format PID to char array
  byte strPidSize = 4;
  char strPid[strPidSize];
  String strPidtemp = String(pid, HEX);
  strPidtemp.toUpperCase();
  if (strPidtemp.length() == 1) {
    strPidtemp = "0" + strPidtemp + " ";
  }
  strPidtemp.toCharArray(strPid, strPidSize);
  
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
    while (Serial.available() == 0); 
    // seek to line of returned data
    while (false == Serial.find("4"));
    // print time 
    OBDLog.print(millis() - startTime);
    OBDLog.print(",");
    //OBDLog.write('4');
    OBDLog.print('0');
    OBDLog.print(mode);
    OBDLog.print(strPid);
    OBDLog.print(",");
    // seek to returned hex data
    while (false == Serial.find(strPid));
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
