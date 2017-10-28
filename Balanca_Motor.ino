#include <Ultrasonic.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SPI.h>
#include "HX711.h"
#include <Servo.h>

 

const int stepsPerRevolution = 500;


//inicializa as portas do motor de passo
#define IN_1 4
#define IN_2 5
#define IN_3 6
#define IN_4 7
#define SPEED_ROTATION 12

//wifi
const int pino_w = 0;



//inicializa as variaveis do rfid

#define SS_PIN 10
#define SCK 13
#define MOSI 11
#define MISO 12
#define RST_PIN 9

// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN);


//inicializa portas do sensor ultrasonico
#define trigger 3
#define echo 2
boolean isOpen;
boolean isService;

#include <Stepper.h> 
 
 
//Inicializa a biblioteca utilizando as portas de 8 a 11 para 
//ligacao ao motor 
Stepper myStepper(stepsPerRevolution, 8,10,9,11); 
//tags

#define TAG_1 "45 5C 07 88"
#define TAG_2 "F9 C7 95 A5"

//inicia sensor de distancia
Ultrasonic ultrasonic(trigger, echo);




//weight


#define DOUT  6
#define CLK  7





HX711 scale(DOUT, CLK);

float calibration_factor = -103.5;

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


bool shakeIt;
void setup()
{
  shakeIt=false;
 myStepper.setSpeed(50);
	scaleSetup();

	//lcd.InitLCD(); //Intializing LCD
	Serial.begin(9600);
}


void loopScale() {

	scale.set_scale(calibration_factor); //Adjust to this calibration factor

	Serial.print("Reading: ");
	Serial.print(scale.get_units(), 1);
 if(scale.get_units()>5){

  //shakeIt=true;
  
 //}else{
   shakeIt=false;

 }
	Serial.print(" gramas"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
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
     else if(temp == 'm')
     shakeIt=true;
	}

}

void loop() {

	loopScale();
if(shakeIt){
//for (int i = 10; i<=60; i=i+30)
 //{
 myStepper.step(100);
 //}


  
}

 
}


