#include <HX711_ADC.h>



/*
Equivalencia das saidas Digitais entre NodeMCU e ESP8266 (na IDE do Arduino)
NodeMCU - ESP8266
D0 = 16;
D1 = 5;
D2 = 4;
D3 = 0;
D4 = 2;
D5 = 14;
D6 = 12;
D7 = 13;
D8 = 15;
D9 = 3;
D10 = 1;

source : https://bitbucket.org/MarcoRabelo/esp8266/src/8b530e758b97/06-MQTT_WiFiManager_Rele/?at=master
*/

#include <FS.h>                 
#include <ESP8266WiFi.h>        //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>        //https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>
#include <EEPROM.h>

#include <RDM6300.h>


#include <Stepper.h>
#include <SoftwareSerial.h>
#include <SPI.h>

#define DEBUG                   //Se descomentar esta linha vai habilitar a 'impressão' na porta serial

#define servidor_mqtt             "m12.cloudmqtt.com"
#define servidor_mqtt_porta       "19337"
#define servidor_mqtt_usuario     "dmsjvruj" 
#define servidor_mqtt_senha       "miXaqhWnTVA9"  
#define mqtt_topico_sub           "esp8266/pincmd" 

#define mqtt_topico_pub           "esp8266/up"  //testing

//Declaração do pino que será utilizado e a memória alocada para armazenar o status deste pino na EEPROM
#define pino                      2                   //Pino que executara a acao dado no topico "esp8266/pincmd" e terá seu status informado no tópico "esp8266/pinstatus"
#define memoria_alocada           8                   //Define o quanto sera alocado na EEPROM (valores entre 4 e 4096 bytes)
WiFiClient espClient;                                 //Instância do WiFiClient
PubSubClient client(espClient);                       //Passando a instância do WiFiClient para a instância do PubSubClient

uint8_t statusAnt   =             0;                  //Variável que armazenará o status do pino que foi gravado anteriormente na EEPROM
bool precisaSalvar  =             false;              //Flag para salvar os dados

uint8_t horarioGato1 = 12;
uint8_t horarioGato2 = 16;
uint8_t quantidadeGato1 = 100;
uint8_t quantidadeGato2 = 200;




















// #define TagBlue "18 0 39 31 46"
// #define TagGreen "12 0 7B 1B D0"

//Inicializa a serial nos pinos 2 (RX) e 3 (TX)
// SoftwareSerial RFID(4, 5);
// uint8_t Payload[6]; // used for read comparisons

// RDM6300 RDM6300(Payload);
// uint8_t tagBlue[6] = {0x18, 0x0, 0x39, 0x31, 0x46};

int relayDoor = 0;

int isOnTime=LOW;

// unsigned long currentMillisGlobal;

int isOpenState = LOW;
int openInterval;// = 5000;
long previousOpenMillis = 0;

int relayDispenser = 16; //port
int shakeInterval = 200;
long previousShakenMillis = 0;
int isShaking = LOW;

int toDispense = 0;
int weight = 0;

// #define DOUT 6
// #define CLK 7

// HX711 scale(DOUT, CLK);

// float calibration_factor = -110; //-7050 worked for my 440lb max scale setup


















//Função para imprimir na porta serial
void imprimirSerial(bool linha, String mensagem){
  #ifdef DEBUG
    if(linha){
      Serial.println(mensagem);
    }else{
      Serial.print(mensagem);
    }
  #endif
}

//Função de retorno para notificar sobre a necessidade de salvar as configurações
void precisaSalvarCallback() {
  imprimirSerial(true, "As configuracoes tem que ser salvas.");
  precisaSalvar = true;
}

//Função que reconecta ao servidor MQTT
void reconectar() {
  //Repete até conectar
  while (!client.connected()) {
    imprimirSerial(false, "Tentando conectar ao servidor MQTT...");
    
    //Tentativa de conectar. Se o MQTT precisa de autenticação, será chamada a função com autenticação, caso contrário, chama a sem autenticação. 
    bool conectado = strlen(servidor_mqtt_usuario) > 0 ?
                     client.connect("ESP8266Client", servidor_mqtt_usuario, servidor_mqtt_senha) :
                     client.connect("ESP8266Client");

    if(conectado) {
      imprimirSerial(true, "Conectado!");
      //Subscreve para monitorar os comandos recebidos
      client.subscribe(mqtt_topico_sub, 1); //QoS 1
    } else {
      imprimirSerial(false, "Falhou ao tentar conectar. Codigo: ");
      imprimirSerial(false, String(client.state()).c_str());
      imprimirSerial(true, " tentando novamente em 5 segundos");
      //Aguarda 5 segundos para tentar novamente
      delay(5000);
    }
  }
}

//Função que verifica qual foi o último status do pino antes do ESP ser desligado
void lerStatusAnteriorPino(){
  EEPROM.begin(memoria_alocada);  //Aloca o espaco definido na memoria
  statusAnt = EEPROM.read(0);     //Le o valor armazenado na EEPROM e passa para a variável "statusAnt"
  if(statusAnt > 1){
    statusAnt = 0;                //Provavelmente é a primeira leitura da EEPROM, passando o valor padrão para o pino.
    EEPROM.write(0,statusAnt);
  }
  digitalWrite(pino, statusAnt);
  EEPROM.end();
}

//Função que grava status do pino na EEPROM
void gravarStatusPino(uint8_t statusPino){
  EEPROM.begin(memoria_alocada);
  EEPROM.write(0, statusPino);
  EEPROM.end();
}

void gravarHorarioGato1(uint8_t dado) {
  EEPROM.begin(memoria_alocada);
  EEPROM.write(1, dado);
  EEPROM.end();
}
void gravarHorarioGato2(uint8_t dado) {
  EEPROM.begin(memoria_alocada);
  EEPROM.write(2, dado);
  EEPROM.end();
}

void gravarQuantidadeGato1(uint8_t dado) {
  EEPROM.begin(memoria_alocada);
  EEPROM.write(3, dado);
  EEPROM.end();
}

void gravarQuantidadeGato2(uint8_t dado) {
  EEPROM.begin(memoria_alocada);
  EEPROM.write(4, dado);
  EEPROM.end();
}


//Função que será chamada ao receber mensagem do servidor MQTT
void retorno(char* topico, byte* mensagem, unsigned int tamanho) {
  //Convertendo a mensagem recebida para string
  mensagem[tamanho] = '\0';
  String strMensagem = String((char*)mensagem);
  strMensagem.toLowerCase();
  //float f = s.toFloat();
  
  imprimirSerial(false, "Mensagem recebida! Topico: ");
  imprimirSerial(false, topico);
  imprimirSerial(false, ". Tamanho: ");
  imprimirSerial(false, String(tamanho).c_str());
  imprimirSerial(false, ". Mensagem: ");
  imprimirSerial(true, strMensagem);



//  imprimirSerial(false, "Status do pino antes de processar o comando: ");
//  imprimirSerial(true, String(digitalRead(pino)).c_str());
    imprimirSerial(false, "Status before command: ");
       if(digitalRead(pino)==0){      
    imprimirSerial(true, "closed" );
     }else{      
    imprimirSerial(true, "open" );
     }

     
  
  if(strMensagem == "open"){
    imprimirSerial(true, "Opening...");
    digitalWrite(pino, HIGH);
    gravarStatusPino(HIGH);
    client.publish(mqtt_topico_pub,"isopen");
  }else if(strMensagem == "close"){
    imprimirSerial(true, "Closing...");
    digitalWrite(pino, LOW);
    gravarStatusPino(LOW);
    
    client.publish(mqtt_topico_pub,"isclosed");
  }else if(strMensagem.startsWith("timecat1-") > 0){
    
    strMensagem.replace("timecat1-", "");
    imprimirSerial(false, "New time Cat1: ");
    imprimirSerial(true, strMensagem );
    
    gravarHorarioGato1(strMensagem.toInt());

//String temp123 ="tcat1-"+strMensagem;
//byte tempLenght = 12;
//    if (client.publish(mqtt_topico_pub, temp123,tempLenght)){
//      
//    imprimirSerial(true, "published" );
//    }else{
//      
//    imprimirSerial(true, "not published" );
//    }
    

  }else if(strMensagem == "check"){
  EEPROM.begin(memoria_alocada);

    imprimirSerial(true, "Checking all values: " );
    
     imprimirSerial(false, "Door is " );
     if(EEPROM.read(0)==0){
      
    imprimirSerial(true, "closed" );
     }else{
      
    imprimirSerial(true, "open" );
     }
     
    imprimirSerial(false, "Time Cat1: ");
    imprimirSerial(true, String(EEPROM.read(1)) );
    
    imprimirSerial(false, "Time Cat2: ");
    imprimirSerial(true, String(EEPROM.read(2)) );

    
    imprimirSerial(false, "Quantity Cat1: ");
    imprimirSerial(true, String(EEPROM.read(3)) );
    
    imprimirSerial(false, "Quantity Cat2: ");
    imprimirSerial(true, String(EEPROM.read(4)) );
  
  EEPROM.end();

    
  
  }else if(strMensagem == "format"){

    SPIFFS.format();
  }
//  else{
//    imprimirSerial(true, "Opening or closing...");
//    digitalWrite(pino, !digitalRead(pino));
//    gravarStatusPino(digitalRead(pino));
//  }
//  
  imprimirSerial(false, "Status after command: ");
       if(digitalRead(pino)==0){      
    imprimirSerial(true, "closed" );
     }else{      
    imprimirSerial(true, "open" );
     }
//  imprimirSerial(true, String(digitalRead(pino)).c_str());
  

}



void setup(){
  setupNode();


  pinMode(relayDoor, OUTPUT);
  pinMode(relayDispenser, OUTPUT);
  // scaleSetup();
  // reset();

  //UpdateWeight();
  Serial.begin(9600);
  // mfrc522.PCD_Init();


}

//Função inicial (será executado SOMENTE quando ligar o ESP)
void setupNode() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  imprimirSerial(true, "...");

  //Fazendo o pino ser de saída, pois ele irá "controlar" algo.
  pinMode(pino, OUTPUT);
  
  //Formatando a memória interna
  //(descomente a linha abaixo enquanto estiver testando e comente ou apague quando estiver pronto)
  //SPIFFS.format();

  //Iniciando o SPIFSS (SPI Flash File System)
  imprimirSerial(true, "Iniciando o SPIFSS (SPI Flash File System)");
  if (SPIFFS.begin()) {
    imprimirSerial(true, "Sistema de arquivos SPIFSS montado!");
    if (SPIFFS.exists("/config.json")) {
      //Arquivo de configuração existe e será lido.
      imprimirSerial(true, "Abrindo o arquivo de configuracao...");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        imprimirSerial(true, "Arquivo de configuracao aberto.");
        size_t size = configFile.size();
        
        //Alocando um buffer para armazenar o conteúdo do arquivo.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
            //Copiando as variáveis salvas previamente no aquivo json para a memória do ESP.
            imprimirSerial(true, "arquivo json analisado.");
            strcpy(servidor_mqtt, json["servidor_mqtt"]);
            strcpy(servidor_mqtt_porta, json["servidor_mqtt_porta"]);
            strcpy(servidor_mqtt_usuario, json["servidor_mqtt_usuario"]);
            strcpy(servidor_mqtt_senha, json["servidor_mqtt_senha"]);
            strcpy(mqtt_topico_sub, json["mqtt_topico_sub"]);

        } else {
          imprimirSerial(true, "Falha ao ler as configuracoes do arquivo json.");
        }
      }
    }
  } else {
    imprimirSerial(true, "Falha ao montar o sistema de arquivos SPIFSS.");
  }
  //Fim da leitura do sistema de arquivos SPIFSS

  //Parâmetros extras para configuração
  //Depois de conectar, parameter.getValue() vai pegar o valor configurado.
  //Os campos do WiFiManagerParameter são: id do parâmetro, nome, valor padrão, comprimento
  WiFiManagerParameter custom_mqtt_server("server", "Servidor MQTT", servidor_mqtt, 40);
  WiFiManagerParameter custom_mqtt_port("port", "Porta", servidor_mqtt_porta, 6);
  WiFiManagerParameter custom_mqtt_user("user", "Usuario", servidor_mqtt_usuario, 20);
  WiFiManagerParameter custom_mqtt_pass("pass", "Senha", servidor_mqtt_senha, 20);
  WiFiManagerParameter custom_mqtt_topic_sub("topic_sub", "Topico para subscrever", mqtt_topico_sub, 30);

  //Inicialização do WiFiManager. Uma vez iniciado não é necessário mantê-lo em memória.
  WiFiManager wifiManager;

  //Definindo a função que informará a necessidade de salvar as configurações
  wifiManager.setSaveConfigCallback(precisaSalvarCallback);
  
  //Adicionando os parâmetros para conectar ao servidor MQTT
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_mqtt_topic_sub);


  if (!wifiManager.autoConnect("SmartCat", "123456789")) {
    imprimirSerial(true, "Falha ao conectar. Excedeu o tempo limite para conexao.");
    delay(3000);
    //Reinicia o ESP e tenta novamente ou entra em sono profundo (DeepSleep)
    ESP.reset();
    delay(5000);
  }

  imprimirSerial(true, "Conectado");

  //Lendo os parâmetros atualizados
  strcpy(servidor_mqtt, custom_mqtt_server.getValue());
  strcpy(servidor_mqtt_porta, custom_mqtt_port.getValue());
  strcpy(servidor_mqtt_usuario, custom_mqtt_user.getValue());
  strcpy(servidor_mqtt_senha, custom_mqtt_pass.getValue());
  strcpy(mqtt_topico_sub, custom_mqtt_topic_sub.getValue());

  //Salvando os parâmetros informados na tela web do WiFiManager
  if (precisaSalvar) {
    imprimirSerial(true, "Salvando as configuracoes");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["servidor_mqtt"] = servidor_mqtt;
    json["servidor_mqtt_porta"] = servidor_mqtt_porta;
    json["servidor_mqtt_usuario"] = servidor_mqtt_usuario;
    json["servidor_mqtt_senha"] = servidor_mqtt_senha;
    json["mqtt_topico_sub"] = mqtt_topico_sub;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      imprimirSerial(true, "Houve uma falha ao abrir o arquivo de configuracao para incluir/alterar as configuracoes.");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
  }

  imprimirSerial(false, "IP: ");
  imprimirSerial(true, WiFi.localIP().toString());

  //Informando ao client do PubSub a url do servidor e a porta.
  int portaInt = atoi(servidor_mqtt_porta);
  client.setServer(servidor_mqtt, portaInt);
  client.setCallback(retorno);
  
  //Obtendo o status do pino antes do ESP ser desligado
  lerStatusAnteriorPino();
}



void scaleSetup()
{

  // Serial.println("HX711 calibration sketch");
  // Serial.println("Remove all weight from scale");
  // Serial.println("After readings begin, place known weight on scale");
  // Serial.println("Press + or a to increase calibration factor");
  // Serial.println("Press - or z to decrease calibration factor");

  // scale.set_scale();
  // scale.tare(); //Reset the scale to 0

  // long zero_factor = scale.read_average(); //Get a baseline reading
  //Serial.print("Zero factor: ");           //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  //Serial.println(zero_factor);
}


void loopMenu(){
  // if (Serial.available())
  // {
  //   char temp = Serial.read();
  //   if (temp == 'r'){
  //     reset();
  //   }
  // }
}

void reset(){

  // Serial.println("Configuracoes resetadas com sucesso");


  // openInterval=3000;
  // previousOpenMillis = currentMillisGlobal;
  
  // toDispense = 200;
}



void loop(){
  loopNode();


  //   loopMenu();
  // delay(100);
 // loopScale();
  //UpdateWeight();
  //Dispenser();
  //Door();
  // Serial.print("On time: ");
  // Serial.println(IsOnTime());
  // if (moduleRFID())
  // {
  //   Serial.println("Abrindo...");
  // }
  // else
  // {
  //   Serial.println("Rejeitado..");
  // }

}
void loopNode() {
  if (!client.connected()) {
    reconectar();
  }
  client.loop();
}




 