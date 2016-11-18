#include <Stepper.h>
#include <SoftwareSerial.h>
#include <Ultrasonic.h>
//#include <CustomStepper.h>  



//inicializa as portas do motor de passo 
#define IN_1 51
#define IN_2 3
#define IN_3 4
#define IN_4 5
#define SPEED_ROTATION 12

//inicializa portas do sensor ultrasonico
#define trigger 12
#define echo 13

//iniciando portas rx e tx

#define RX 10
#define TX 35


//tags

#define TAG_1 "180039314656"
#define TAG_2 "030073B9B47D"

   
const int stepsPerRevolution = 500; 
Stepper myStepper(stepsPerRevolution, IN_1,IN_3,IN_2,IN_4);  

//inicia sensor de distancia
Ultrasonic ultrasonic(trigger, echo);

//iniciando o sensor RFID nas portas Rx e TX
SoftwareSerial RFID(RX, TX); // RX and TX


//variaveis globais

String msg,last; 
char c;
  
void setup() 
{   
  //pinMode(RX,OUTPUT);
   
  Serial.begin(9600);

  RFID.begin(9600);

  myStepper.setSpeed(60);

} 
  
void loop() 
{ 

  boolean isOpen = false;
  boolean reconheceu = rfidMod();
  delay(1000);

 if(RFID.available() > 0){
  if(reconheceu && distance() < 10){
    Serial.println("Abrindo");
     motor(180);
    isOpen = true;
  }
  
}
//delay de 1 min para fechar

if(isOpen == true && distance() > 10){
   Serial.println("Fechando..");
  delay(30000);
  motorClose(180);
}

 
  

}
void motor(int graus){

  //Gira o motor no sentido horario a 90 graus 2 vezes
  myStepper.step(converteGraus(graus)); 
 delay(2000);

}
void motorClose(int graus){

  //Gira o motor no sentido horario a 90 graus 2 vezes

  myStepper.step(converteGraus(graus)); 
 
 delay(2000);

}
boolean rfidMod(){
  
if(RFID.available() > 0){
  while(RFID.available()>0){
      c=RFID.read(); 
      msg += c;
  }
  msg=msg.substring(1,13);
  if(msg != last){ 
   if (msg.length()>1){
       last=msg;
   } 
  }
  
  if(msg == TAG_1){
    Serial.println("TAG Aceita");
    Serial.print("TAG: ");
    Serial.println(msg);
    msg="";
    Serial.flush();

    return true;
  }else{
    Serial.println("Negado Para:");
    Serial.println(msg);
    msg="";
    Serial.flush();
    return false;
  }
}
    
}
float distance(){ 
  float t = ultrasonic.Timing();
  float dt = t * 0.034 / 2;
  Serial.print("Distancia: ");
  Serial.println(dt);
  delay(500);

  return dt;
}
int converteGraus(int graus){
  double valorMotor = 5.689 * graus;
  return valorMotor;
}


