#include <SoftwareSerial.h>
#include <Ultrasonic.h>
#include <CustomStepper.h>  


//inicializa as portas do motor de passo 
#define IN_1 22
#define IN_2 5
#define IN_3 6
#define IN_4 7
#define SPEED_ROTATION 12

//inicializa portas do sensor ultrasonico
#define trigger 12
#define echo 13

//iniciando portas rx e tx

#define RX 10
#define TX 35


//tags

#define TAG_1 "180039314656"

   
//Define os parametros iniciais de ligacao do motor de passo  
CustomStepper stepper(22, 5, 6, 7, (byte[]){8/*numero de passos*/, B1000, B1100, B0100, 
B0110, B0010, B0011, B0001, B1001}, 4075.7728395, SPEED_ROTATION, CW);  

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

  //Define a velocidade do motor  
  stepper.setRPM(SPEED_ROTATION);  
  
  //Define o numero de passos por rotacao  
  stepper.setSPR(4075.7728395);  

} 
  
void loop() 
{ 


  boolean reconheceu = rfidMod();
  delay(1000);

  if(reconheceu && distance() < 10){
    Serial.println("Entrou");
    
  }
  

}
void motor(int angulo){

 //Serial.println(stepper.isDone());
 if (stepper.isDone())  
 {  
   //Intervalo entre acionamentos  
    delay(1000);  
   //Define o sentido de rotacao (CW = Horario)  
   stepper.setDirection(CW);  
   //Define o angulo de rotacao  
   stepper.rotateDegrees(angulo);  
  }  

  stepper.run();

}
boolean rfidMod(){
  
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
    Serial.println("Aceita");
    Serial.print("TAG: ");
    Serial.println(msg);
    msg="";
    Serial.flush();

    return true;
  }

  return false;
    
}
float distance(){
  float t = ultrasonic.Timing();
  float dt = t * 0.034 / 2;
  Serial.print("Distancia: ");
  Serial.println(dt);
  delay(500);

  return dt;
}



