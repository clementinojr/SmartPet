#include <Ultrasonic.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>


//inicializa as portas do motor de passo
#define IN_1 4
#define IN_2 5
#define IN_3 6
#define IN_4 7
#define SPEED_ROTATION 12


int medir = -1;




//inicializa portas do sensor ultrasonico
#define trigger 3
#define echo 2
boolean isOpen;



const int stepsPerRevolution = 500;
Stepper myStepper(stepsPerRevolution, IN_1, IN_3, IN_2, IN_4);

//inicia sensor de distancia


Ultrasonic ultrasonic(trigger, echo);


SoftwareSerial RFID(10, 9);
void setup()
{
  //Inicializa a serial para o leitor RDM6300
  RFID.begin(9600);
  //Inicializa a serial para comunicacao com o PC
  Serial.begin(9600);
  //Informacoes iniciais
  Serial.println("Leitor RFID RDM6300\n");
  isOpen = false;
  myStepper.setSpeed(60);
    
}


String leitura;

void loop()
{ 
  float distanceVal = distance();
  boolean reconheceu = false;
  //rfidMod();
  leitura = "0";


   
//medir = RFID.available();

  while (RFID.available() > 0 )//&& leitura=="0"
  {
    //digitalWrite(Led, HIGH);
    leitura = RFID.read();
  Serial.print("RFID: ");
      Serial.println(leitura);
    
  }

  
  Serial.print("RFID: ");
  Serial.println(leitura);
  
  if(leitura != "0" &&!isOpen){
    Serial.println("Abrindo...");
      motor(-180);
     isOpen = true;
     //delay(2000);
    }
  if(isOpen == true && distanceVal >20 ){
      Serial.println("Fechando..");
      //delay(1000);
      motorClose(-180);
      isOpen = false;
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
// 
//medir = RFID.available();
//
//Serial.println(medir);
//  if (medir> 0){
//      return true;
//      medir=0;
//  }else{
//    return false;
//  }
//  

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
