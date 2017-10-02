#include <Ultrasonic.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


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


//tags

#define TAG_1 "45 5C 07 88"
#define TAG_2 "F9 C7 95 A5"

const int stepsPerRevolution = 500;
Stepper myStepper(stepsPerRevolution, IN_1, IN_3, IN_2, IN_4);
//inicia sensor de distancia
Ultrasonic ultrasonic(trigger, echo);


// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)

Adafruit_PCD8544 display = Adafruit_PCD8544(A5, A4, A3, A2, A1);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16


void setup()
{
	display.begin();
	display.setContrast(50);
	display.clearDisplay();
	display.setRotation(1); //rotate 90

	display.display();
	display.setTextSize(1);
	display.setTextColor(BLACK);

	//lcd.InitLCD(); //Intializing LCD
	Serial.begin(9600);
	isOpen = false;
	isService = false;
	myStepper.setSpeed(60);
	SPI.begin();// Inicia  SPI bus  
	mfrc522.PCD_Init(); // Inicia MFRC522
}


float distanceVal;
boolean reconheceu;

void refreshDisplay() {


	display.clearDisplay();
	display.setCursor(0, 0);
	display.print("Dis:");
	display.println((int)distanceVal);
	display.print("Tag:");
	display.println(reconheceu);

	display.print("Serv:");
	display.println(isService);
	display.print("Wifi:");
	display.println(false);//todo
	display.print("Wgt:");
	display.println(false);//todo

	display.display();




}



void loop(){
	distanceVal = distance();
	reconheceu = rfidMod();


	refreshDisplay();
	if (isOpen) {
		if (isService) {
			if (digitalRead(pino_w) == HIGH) {

				Serial.println("Pino w High");
				fechar();
				isService = false;

				Serial.println("Fora de servico");
			}
		}
		else {
			if (distanceVal >20) {
				fechar();
			}
		}
	}
	else {

		if (reconheceu) {
			abrir();
		}
		else if (digitalRead(pino_w) == LOW) {
			Serial.println("Pino w Low");
			abrir();
			isService = true;

			Serial.println("Em servico");
		}
	}
}

void abrir() {
	Serial.println("Abrindo...");
	motor(-180);//abre
	isOpen = true;
}
void fechar() {
	Serial.println("Fechando..");
	motor(180);
	//motorClose(-180);//fecha
	isOpen = false;
}




void motor(int graus) {
	myStepper.step(converteGraus(graus));
	delay(2000);

}
//void motorClose(int graus) {
//	myStepper.step(-converteGraus(graus));
//	delay(2000);
//
//}
boolean rfidMod() {
	if (mfrc522.PICC_IsNewCardPresent()) {
		Serial.println("Aceito");
		return true;
	}
	return false;
}
float distance() {
	long t = ultrasonic.timing();
	long dt = t * 0.034 / 2;

	Serial.print("Distancia: ");
	Serial.println(dt);
	delay(500);

	return dt;
}
int converteGraus(int graus) {
	double valorMotor = 5.689 * graus;
	return valorMotor;
}
