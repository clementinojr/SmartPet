#include <RDM6300.h>

#include "HX711.h"

#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>

// #define TagBlue "18 0 39 31 46"
// #define TagGreen "12 0 7B 1B D0"

//Inicializa a serial nos pinos 2 (RX) e 3 (TX)
SoftwareSerial RFID(8, 9);
uint8_t Payload[6]; // used for read comparisons

RDM6300 RDM6300(Payload);
uint8_t tagBlue[6] = {0x18, 0x0, 0x39, 0x31, 0x46};

int relayDoor = 3;

int isOnTime=LOW;

unsigned long currentMillisGlobal;

int isOpenState = LOW;
int openInterval;// = 5000;
long previousOpenMillis = 0;

int relayDispenser = 4; //port
int shakeInterval = 200;
long previousShakenMillis = 0;
int isShaking = LOW;

int toDispense = 0;
int weight = 0;

#define DOUT 6
#define CLK 7

HX711 scale(DOUT, CLK);

float calibration_factor = -110; //-7050 worked for my 440lb max scale setup

void setup()
{
  RFID.begin(9600);
  pinMode(relayDoor, OUTPUT);
  pinMode(relayDispenser, OUTPUT);
  scaleSetup();
  reset();

  //UpdateWeight();
  Serial.begin(9600);
  // mfrc522.PCD_Init();
}

void scaleSetup()
{

  // Serial.println("HX711 calibration sketch");
  // Serial.println("Remove all weight from scale");
  // Serial.println("After readings begin, place known weight on scale");
  // Serial.println("Press + or a to increase calibration factor");
  // Serial.println("Press - or z to decrease calibration factor");

  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  //Serial.print("Zero factor: ");           //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  //Serial.println(zero_factor);
}


void loopMenu(){
  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == 'r'){
      reset();
    }
  }
}

void reset(){

  Serial.println("Configuracoes resetadas com sucesso");


  openInterval=3000;
  previousOpenMillis = currentMillisGlobal;
  
  toDispense = 200;
}
void loop()
{
  loopMenu();
  delay(100);
 // loopScale();
  //UpdateWeight();
  //Dispenser();
  //Door();
  Serial.print("On time: ");
  Serial.println(IsOnTime());
  // if (moduleRFID())
  // {
  //   Serial.println("Abrindo...");
  // }
  // else
  // {
  //   Serial.println("Rejeitado..");
  // }
}

void Dispenser()
{

  if (weight < toDispense && weight > -25)
  {
    unsigned long currentMillis = millis();


    if (currentMillis - previousShakenMillis > shakeInterval)
    {
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
  }
  else
  {
    toDispense = 0;
  }
}


bool IsOnTime(){

  currentMillisGlobal = millis();
  if (currentMillisGlobal - previousOpenMillis > openInterval)
  {
    
    // openInterval=0;
    
    return false;
  }else{
    return true;
  }
  
}

void Door(bool sel)
{
    // if (isOpenState == LOW){
    //   isOpenState = HIGH;
    // }else{
    //   isOpenState = LOW;
    //  }
    
    digitalWrite(relayDoor, isOpenState);
}

void UpdateWeight()
{

  weight = scale.get_units();
}

void loopScale()
{

  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" grams"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  // Serial.print(" calibration_factor: ");
  // Serial.print(calibration_factor);
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

bool moduleRFID()
{
  bool known = false;
  while (RFID.available() > 0)
  {
    uint8_t c = RFID.read();
    if (RDM6300.decode(c))
    {
      Serial.print("ID TAG: ");
      //Mostra os dados no serial monitor
      known = true;
      for (int i = 0; i < 5; i++)
      {
        Serial.print(Payload[i], HEX);
        Serial.print(tagBlue[i], HEX);
        if ((Payload[i]) == (tagBlue[i]))
        {
          Serial.println("Deu");
        }
        else
        {
          known = false;
        }
      }
      Serial.println();
    }
  }

  for (int i = 0; i < 5; i++)  {
    Payload[i] = 0x0;
  }
  return known;
}