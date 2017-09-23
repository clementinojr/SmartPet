#include <Ultrasonic.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>


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
void setup()
{
  Serial.begin(9600);
  isOpen = false;
  isService = false;
  myStepper.setSpeed(60);
  SPI.begin();// Inicia  SPI bus  
  mfrc522.PCD_Init(); // Inicia MFRC522
}

void loop()
{
  float distanceVal = distance();
  boolean reconheceu = rfidMod();


if(isOpen){
  if(isService){
    if(digitalRead(pino_w)== HIGH){
      
    Serial.println("Pino w High");
      fechar();
      isService=false;
      
    Serial.println("Fora de servico");
    }  
  }else{
    if(distanceVal >20 ){
      fechar();
    }
  }  
}else{

 if(reconheceu){
    abrir();    
  }else if(digitalRead(pino_w)== LOW){
    Serial.println("Pino w Low");
    abrir();
    isService=true;
    
    Serial.println("Em servico");
   }

  
}
}

void abrir(){
    Serial.println("Abrindo...");
      motor(-180);//abre
     isOpen = true;  
}
void fechar(){
        Serial.println("Fechando..");
      motorClose(-180);//fecha
      isOpen = false;
}

void motor(int graus) {
  myStepper.step(converteGraus(graus));
  delay(2000);

}
void motorClose(int graus) {
  myStepper.step(-converteGraus(graus));
  delay(2000);

}
boolean rfidMod() {
  if(mfrc522.PICC_IsNewCardPresent()){
      Serial.println("Aceito");
      return true;
  }
  return false;
}
float distance(){
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
