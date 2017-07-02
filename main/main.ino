#include "wifi.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Globals
int RFIDResetPin = 13;
int state = LOW;
boolean player = false;

// Web
const int httpPort = 3000;
const char *host = "10.0.0.176";

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

  pinMode(4, INPUT);
  pinMode(5, OUTPUT);

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

  Serial.println(tagString);

  clearTag();
}

void internalLoop() {
  int newState = digitalRead(4);
  digitalWrite(5, newState);

  if (state != newState) {
    state = newState;
    if (newState == HIGH) {
      if (player) {
        tone(12, 587, 250);
        delay(250);
        tone(12, 880, 500);
        delay(500);
      } else {
        tone(12, 659, 250);
        delay(250);
        tone(12, 988, 500);
        delay(500);
      }
      player = !player;
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
