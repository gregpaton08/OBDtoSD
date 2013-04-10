#include <Arduino.h>
#include <SD.h>
#include "OBDLib.h"

File OBDLog;
unsigned long startTime;

OBDLib obd;

#define SD_SS 4
#define BLE_REQN 9
#define MEGA_SS 53


void setup() {
  // set BAUD rate
  Serial.begin(38400);
  
  pinMode(MEGA_SS, OUTPUT);
  digitalWrite(MEGA_SS, HIGH);
  
  // BLE
  pinMode(BLE_REQN, OUTPUT);
  digitalWrite(BLE_REQN, HIGH);
  
  SDInit(); 
  
  obd.init(); 
  
  //obd.getSupportedPids();
  
  // wait a second
  delay(1000);
  // okay
  
  // Keep track of log time
  startTime = millis();
}


void loop() {
  // RPM
  logMode01PID(0x0C);
  // Speed
  logMode01PID(0x0D);
  // MAF
  logMode01PID(0x10);
  
  // flush data to SD card
  OBDLog.flush();
}


void SDInit() {
  // Set slave select pin for SD Card
  pinMode(SD_SS, OUTPUT);
  if (false == SD.begin(SD_SS)) return;
 
  // Determine file name
  // File name must be 8 characters or less
  char filename[13];
  for (uint16_t index = 1; index < 65535; index++) {
    sprintf(filename, "OBD%05d.csv", index);
    if (false == SD.exists(filename)) {
      break;
    }
  }
  
  // Open file
  OBDLog = SD.open(filename, FILE_WRITE);
}


void logMode01PID(byte pid) {
  uint8_t len = 0;
  uint8_t pidResSize = 10;
  char pidRes[pidResSize];
  
  // check if supported
  //if (false == obd.isPidSupported(0x01, pid))
  //  return;
  
  // Query PID
  obd.sendCMD(0x01, pid);
  
  uint8_t strPidSize = 4;
  char strPid[strPidSize];
  String strPidtemp = String(pid, HEX);
  strPidtemp.toUpperCase();
  if (strPidtemp.length() == 1) {
      strPidtemp = "0" + strPidtemp;
  }
  strPidtemp.toCharArray(strPid, strPidSize);
  while (false == Serial.find(strPid));
    
  // Wait until data is available
  //if (false == obd.waitForPid(pid, 200))
  //  return;
  // Print time 
  OBDLog.print(millis() - startTime);
  OBDLog.print(",");
  //OBDLog.print(0x01, HEX);
  if (pid < 0x10)
    OBDLog.print('0');
  OBDLog.print(pid, HEX);
  OBDLog.print(",");
  OBDLog.flush();
   
  // loop until new line character found
  while (len < pidResSize) {
    unsigned char c = Serial.read();
    if (c == (unsigned char)-1) continue;
    if (c == '\n' || c == '\r') break;
    //OBDLog.write(c);
    pidRes[len] = c;
    ++len;
  }
  // print result to 2 decimal places
  OBDLog.println(obd.pidToDec(pid, pidRes), 2);
  //OBDLog.println("");
}
