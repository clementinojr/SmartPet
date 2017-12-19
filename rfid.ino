//Programa: Leitor RFID RDM6300
//Alteracoes e adaptacoes: Arduino e Cia

//Baseado no programa original de Stephane Driussi


#define TagBlue "18 0 39 31 46"
#define TagGreen "12 0 7B 1B D0"
#include <SoftwareSerial.h>
#include <RDM6300.h>

//Inicializa a serial nos pinos 2 (RX) e 3 (TX)
SoftwareSerial RFID(8, 9);

int Led = 13;
uint8_t Payload[6]; // used for read comparisons

RDM6300 RDM6300(Payload);

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
  while (RFID.available() > 0)
  {
    digitalWrite(Led, HIGH);
    uint8_t c = RFID.read();
    if (RDM6300.decode(c))
    {
      Serial.print("ID TAG: ");
      //Mostra os dados no serial monitor
      for (int i = 0; i < 5; i++) {
        Serial.print(Payload[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
  digitalWrite(Led, LOW);
  delay(100);
}