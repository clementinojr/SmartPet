#include <Ultrasonic.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>


//inicializa as portas do motor de passo
#define IN_1 7
#define IN_2 6
#define IN_3 5
#define IN_4 4
#define SPEED_ROTATION 12

//inicializa as variaveis do rfid

#define SS_PIN 10
#define SCK 13
#define MOSI 11
#define MISO 12
#define RST_PIN 9

// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 


//inicializa portas do sensor ultrasonico
#define trigger 2
#define echo 3



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


  myStepper.setSpeed(60);

   // Inicia  SPI bus
  SPI.begin();
  // Inicia MFRC522
  mfrc522.PCD_Init(); 

}

void loop()
{

  boolean isOpen = false;
  boolean reconheceu = rfidMod();
  

  if(reconheceu){
    Serial.println("Abrindo...");
      motor(180);
     isOpen = true;
     //delay(2000);
    }
  if(isOpen == true){
      Serial.println("Fechando..");
      //delay(1000);
      motorClose(180);
    }



     

}
void motor(int graus) {

  //Gira o motor no sentido horario a 90 graus 2 vezes
  myStepper.step(converteGraus(graus));
  delay(2000);

}
void motorClose(int graus) {

  //Gira o motor no sentido horario a 90 graus 2 vezes

  myStepper.step(-converteGraus(graus));

  delay(2000);

}
boolean rfidMod() {

   // Aguarda a aproximacao do cartao
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return false;
  }
  // Seleciona um dos cartoes
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return false;
  }
  // Mostra UID na serial
  //Serial.print("UID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     //Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  /*Serial.println();
  Serial.print("Mensagem : ");*/
  conteudo.toUpperCase();
  
  // Testa se o cartao1 foi lido
  if (conteudo.substring(1) == TAG_1)
  {
      Serial.println("Aceito");
      return true;
  }
    
    return false;
  
 
}
float distance(){
  long t = ultrasonic.timing();
  loat dt = t * 0.034 / 2;
  Serial.print("Distancia: ");
  Serial.println(dt);
  delay(500);

  return dt;
}
int converteGraus(int graus) {
  double valorMotor = 5.689 * graus;
  return valorMotor;
}


