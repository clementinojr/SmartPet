//Programa: Leitor RFID RDM6300
//Alteracoes e adaptacoes: Arduino e Cia

//Baseado no programa original de Stephane Driussi

#include <SoftwareSerial.h>
#include <rdm630.h>

//Inicializa a serial nos pinos 2 (RX) e 3 (TX)
SoftwareSerial RFID(10, 2);

int Led = 13;
uint8_t Payload[6]; // used for read comparisons

//rdm630 RDM6300(Payload);

void setup()
{
  pinMode(Led, OUTPUT);
  //Inicializa a serial para o leitor RDM6300
  RFID.begin(9600);
  //Inicializa a serial para comunicacao com o PC
  Serial.begin(9600);
  //Informacoes iniciais
  Serial.println("Leitor RFID RDM6300\n");
}

void loop()
{
  //Aguarda a aproximacao da tag RFID
  uint8_t c;
  while (RFID.available() > 0)
  {
    //digitalWrite(Led, HIGH);
      Serial.println(RFID.read());
    
  }
  //Serial.println(c);
 //digitalWrite(Led, LOW);
 //delay(100);
}
