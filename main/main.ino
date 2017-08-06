/**
 * Pin Layout:
 * Speaker:
 * one pin - pin12
 * other pin - GND
 * 
 * Touch sensor:
 * GND - GND
 * out - pin4
 * VDD - 3v3
 * 
 * RFID ID-12LA reader (from bottom to top, when text is facing you)
 * Left side:
 * VCC - 3v3
 * D0 - Serial RX
 * FORM - GND
 * 
 * Right side:
 * GND - GND
 * RES - pin13
 * 
 */

#include "wifi.h"
#include "rest.h"
#include <ESP8266WiFi.h>

#define PLAYER_ONE 0
#define PLAYER_TWO 1

// Consts
const int playerPos = PLAYER_TWO;
const int RFIDResetPin = 13;
const int touchInpuPin = 4;
const int speakerOutputPin = 12;

// Globals
int state = LOW;

void wifi_connect() 
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());  
}

void setup() {
  Serial.begin(9600);

  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);

  pinMode(touchInpuPin, INPUT);

  wifi_connect();
}

char tagString[13];
boolean reading = false;

void loop() {
  resetReader();

  for (int i = 0; i < 150; i++) {
    internalLoop();
    delay(1);
  }
  
  readTag();

  if (strlen(tagString) > 0) {
    tone(speakerOutputPin, 2637, 150);
  }

  if (strlen(tagString) > 0){
    Serial.println(tagString);
    sendRfid(tagString, playerPos);
  }

  clearTag();
}

void internalLoop() {
  int newState = digitalRead(touchInpuPin);

  if (state != newState) {
    state = newState;
    if (newState == HIGH) {
      if (playerPos == PLAYER_ONE) {
        tone(speakerOutputPin, 587, 250);
        delay(250);
        tone(speakerOutputPin, 880, 500);
        delay(500);
      } else {
        tone(speakerOutputPin, 659, 250);
        delay(250);
        tone(speakerOutputPin, 988, 500);
        delay(500);
      }

      sendScore(playerPos);
    }    
  }
}

void readTag() {
  int index = 0;
  
  while(Serial.available()) {
    int readByte = Serial.read(); //read next available byte

    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }
}

void resetReader() {
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  // Need to delay 150 after this
}

void clearTag() {
  for(int i = 0; i < strlen(tagString); i++){
    tagString[i] = 0;
  }
}
