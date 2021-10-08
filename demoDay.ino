#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "HX711.h"

#define doorPin 13

#define dirPin 16
#define stepPin 17

#define stepsPerRevolution 200
BLEScan* pBLEScan;

const int LOADCELL_DOUT_PIN = 22;
const int LOADCELL_SCK_PIN = 23;

int minDistance = -80;

unsigned long millisMotor = millis();
unsigned long millisMotor2 = millis();
int timeMotor = 1000;

HX711 scale;


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if(advertisedDevice.getAddress().toString() == "d0:eb:8c:96:10:8d" ){
          digitalWrite(doorPin, advertisedDevice.getRSSI() > minDistance ? HIGH : LOW);
          Serial.println((String("setDistance") + String(advertisedDevice.getRSSI())).c_str());
          Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
        }
     }
};
void setup() {
  Serial.begin(115200);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(doorPin, OUTPUT);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-8250.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();      
  BLEDevice::init("");
}
void drop(int grams){
  Serial.print("Dropping");
  Serial.println(String(grams).c_str());

  int initialScale = scale.get_units(10);

  
  //get current scale
  
  for (int i = 0; i < stepsPerRevolution; i++) {
    if(millis() - millisMotor > timeMotor){
       digitalWrite(stepPin, HIGH);
       millisMotor = millis();    
    }else if (millis() - millisMotor2 > timeMotor*2){
        digitalWrite(stepPin, LOW);
       millisMotor2 = millis();   
    }
  }
  //if grams+original scale = 
}

int getScale(){
  return -scale.get_units(10)/10;
}

void loop() {
//    drop(1);
    Serial.println((String("setScale") + String(getScale())).c_str());
    pBLEScan = BLEDevice::getScan(); 
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); 
    BLEScanResults foundDevices = pBLEScan->start(1);
     if(Serial.available())
        {
          String temp = Serial.readString();
          if(temp.startsWith("scale.tare")){
            scale.tare(); 
          }else if(temp.startsWith("setMinDistance")){
            temp.replace("setMinDistance","");
            minDistance = temp.toInt();
          }else if(temp.startsWith("drop")){
            temp.replace("drop","");
            drop(temp.toInt());
          }
        }
}
