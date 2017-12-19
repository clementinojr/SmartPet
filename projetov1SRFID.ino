#include <RDM6300.h>


#include "HX711.h"

#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>



#define TAG_1 "45 5C 07 88"
#define TAG_2 "F9 C7 95 A5"


MFRC522 mfrc522(8, 9);


int relayDoor=3;

int isOpenState=LOW;
int openInterval=5000;
long previousOpenMillis = 0; 




int relayDispenser=4;//port
int shakeInterval= 200;
long previousShakenMillis=0;
int isShaking=LOW;



int toDispense=0;
int weight=0;


#define DOUT  6
#define CLK  7


HX711 scale(DOUT, CLK);

float calibration_factor = -110; //-7050 worked for my 440lb max scale setup

void setup()
{

  pinMode(relayDoor, OUTPUT);
  pinMode(relayDispenser, OUTPUT);
  scaleSetup();
  // openInterval=5000;
toDispense=200;

  //UpdateWeight();
  Serial.begin(9600);
  // mfrc522.PCD_Init(); 
}

void scaleSetup() {
  
    Serial.println("HX711 calibration sketch");
    Serial.println("Remove all weight from scale");
    Serial.println("After readings begin, place known weight on scale");
    Serial.println("Press + or a to increase calibration factor");
    Serial.println("Press - or z to decrease calibration factor");
    
    scale.set_scale();
    scale.tare(); //Reset the scale to 0
    
    long zero_factor = scale.read_average(); //Get a baseline reading
    Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
    Serial.println(zero_factor);
    
  
  }


void loop(){
    loopScale();
  UpdateWeight();
   Dispenser();   
    Door(); 

    // boolean reconheceu = rfidMod();
    
    
    // if(reconheceu){
    //   Serial.println("Abrindo...");
    //   }else{
    //     Serial.println("Fechando..");
    // }
    
  }


void Dispenser(){

  if(weight<toDispense&& weight>-25){

    unsigned long currentMillis = millis();
    
    if(currentMillis - previousShakenMillis > shakeInterval) {
      // save the last time you blinked the LED 
      previousShakenMillis = currentMillis;   
      
      // if the LED is off turn it on and vice-versa:
      if (isShaking == LOW)
      isShaking = HIGH;
      else
      isShaking = LOW;
      
      // set the LED with the isOpenSta of the variable:
      digitalWrite(relayDispenser, isShaking);
    }
  }else{
    toDispense=0;
  }
}
  void Door(){
    
    unsigned long currentMillis = millis();
    
    if(currentMillis - previousOpenMillis > openInterval) {
         // save the last time you blinked the LED 
         previousOpenMillis = currentMillis;   
      
         // if the LED is off turn it on and vice-versa:
         if (isOpenState == LOW)
           isOpenState = HIGH;
         else
           isOpenState = LOW;
      
         // set the LED with the isOpenSta of the variable:
         digitalWrite(relayDoor, isOpenState);
       }
    }


    boolean rfidMod() {

      if(mfrc522.PICC_IsNewCardPresent()){
        Serial.println("Aceito");
        return true;
      }
      return false;
      
        //  // Aguarda a aproximacao do cartao
        // if ( ! mfrc522.PICC_IsNewCardPresent()) 
        // {
        //   return false;
        // }
        // // Seleciona um dos cartoes
        // if ( ! mfrc522.PICC_ReadCardSerial()) 
        // {
        //   return false;
        // }
        // // Mostra UID na serial
        // //Serial.print("UID da tag :");
        // String conteudo= "";
        // byte letra;
        // for (byte i = 0; i < mfrc522.uid.size; i++) 
        // {
        //    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //    //Serial.print(mfrc522.uid.uidByte[i], HEX);
        //    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        //    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
        // }
        // /*Serial.println();
        // Serial.print("Mensagem : ");*/
        // conteudo.toUpperCase();
        
        // // Testa se o cartao1 foi lido
        // if (conteudo.substring(1) == TAG_1)
        // {
        //     Serial.println("Aceito");
        //     return true;
        // }
          
        //   return false;
        
       
      }

void UpdateWeight(){

  weight= scale.get_units();
  
}



void loopScale() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" gramas"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 5;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 5;
  }

}
