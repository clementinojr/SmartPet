#include <RDM6300.h>


#include "HX711.h"

#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>



SoftwareSerial RFID(8, 9);


int relePorta=3;
int releXaxu=4;

int peso=0;


#define DOUT  6
#define CLK  7


HX711 scale(DOUT, CLK);

float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup


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


void setup()
{

   pinMode(relePorta, OUTPUT);
    pinMode(releXaxu, OUTPUT);

  scaleSetup();
  

 RFID.begin(9600);
  //lcd.InitLCD(); //Intializing LCD
  Serial.begin(9600);
 
}





void loopScale() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }

}

void loop(){

  loopScale();

  peso= scale.get_units();




  uint8_t c;
  while (RFID.available() > 0)
  {

    digitalWrite(releXaxu, HIGH); 
    
    
    Serial.println(RFID.read());
    
  } 
    
  



  
  
}







//void motorClose(int graus) {
//  myStepper.step(-converteGraus(graus));
//  delay(2000);
//
//}

