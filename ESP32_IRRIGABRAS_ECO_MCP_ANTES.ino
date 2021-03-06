#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "ESPmDNS.h"
#include "ArduinoOTA.h"
#include <SPI.h>
#include "LoRa.h"
#include <Arduino.h>
#include "FS_File_Record.h"
#include "FS_File_Record2.h"
#include "FS_File_Record3.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <Wire.h>


#define LIGA              3
#define DESLIGA           1
#define AVANCO            0
#define REVERSO           22
#define MOLHADO           23
#define RAUX              15
#define RAUXP             2
#define PERCAT            4
#define BOMBA             25


//Adafruit_MCP23017 mcp;
//LEITURA

#define AVREAL            36
#define RTREAL            37
#define PRESS             39
#define PERC              38


//LoRA SPI

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

#define BAND 915E6

#define PABOOST true

void taskDES( void *pvParameters ); //DEFINE TASK

//hw_timer_t *timer = NULL;

//String TIMERTC;

String LoRaData;
String JSONhora;

int rssi;

String TIPO;
String nome;
String eco;
int auxeco = 0;
int inicioH = 0;
int fimH = 0;
int inicioM = 0;
int fimM = 0;

int PROGMEM horaag[4];
int PROGMEM percag[4];
String PROGMEM atuaag[4];
int PROGMEM horaag2[4];
int PROGMEM percag2[4];
String PROGMEM atuaag2[4];
String atuaP[10];
int pos[10];
int percA[10];
String atuaP2[10];
int pos2[10];
int percA2[10];
int PROGMEM contag;
int PROGMEM contpos;
String responseHist;
String GPRS_ID;

const char* ssid = "Soil";
const char* password = "soil2021";
const char* ssidap = "Painel Soil-Irrigabras";
const char* passwordap = "soil2021";

// INPUTS PAGINA WEB

const char* PARAM_INPUT_0 = "Estado";
const char* PARAM_INPUT_0_2 = "sentido";
const char* PARAM_INPUT_0_3 = "seco";
const char* PARAM_INPUT_1 = "percentimetro";
const char* PARAM_INPUT_2_1 = "Estado1";
const char* PARAM_INPUT_2_2 = "sentido1";
const char* PARAM_INPUT_2_3 = "seco1";
const char* PARAM_INPUT_2_4 = "horario1";
const char* PARAM_INPUT_2_5 = "horario2";
const char* PARAM_INPUT_3_1 = "Estado2";
const char* PARAM_INPUT_3_3 = "angulo1";
const char* PARAM_INPUT_3_4 = "angulo2";
const char* PARAM_CONTATORA = "inputTipo";
const char* PARAM_ESPERA = "inputDelay";
const char* PARAM_NOME = "inputNome";
const char* PARAM_SW = "inputSW";
const char* PARAM_DelayPressao = "inputDelaypressao";
const char* PARAM_PERC_H = "percentimetroAH";
const char* PARAM_GPRS_ID = "inputPivotID";//**web
const char* PARAM_NODE_ID = "inputNodeID";
String inputMessage = "100";

int LoRaAdress;

char INWEB[3];
char EstadoAtual[3];
char EstadoAnterior[3];

int flagmem = 0;
int flagMolhado = 0;
int auxRA = 0;
long epoch;
long tempo = 0;
int hoje;
int endloop;
int espera;
int error = 0;
// Estado apresentado na pagina web

String sentidow;
String secow;
String ligadow;
String Snum;

String sentido = "";
String seco = "";
String ligado = "";

String angulo;
String hora;
String horaH;
String horaM;
String horaS;
int horaTS;
int mqttFlag = 0;


int webflag = 0;
String percs;
int percRET = 0;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char botao_on[] PROGMEM = R"rawliteral(<a href="/get?modoeco=off"><button class="bt_eco_on">Eco</button></a>)rawliteral";
const char botao_off[] PROGMEM = R"rawliteral(<a href="/get?modoeco=on"><button class="bt_eco_off">Eco</button></a>)rawliteral";
const char eco_on[] PROGMEM = R"rawliteral(<div class="eco_on"></div>)rawliteral";
const char eco_off[] PROGMEM = R"rawliteral(<div class="eco_off"></div>)rawliteral";


char stats[60];  // para entrada Serial2.

int auxP = 0;
int perc = 0;
int entraperc = 0;

int RETflag = 0;

String STRperc;

int auxPa = 0;

int numw;
int num;
int aux2 = 0;


unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
unsigned long tatual = 0;
unsigned long t2 = 0;
unsigned long t3 = 0;
unsigned long t4 = 0;
unsigned long t5 = 0;
unsigned long t6 = 0;
unsigned long time_on = 0;
unsigned long DelayReliga = 60000;
unsigned long UltimaLeitura = 0;
unsigned long DelayLeitura = 1000;

int DelayPressao = 0;
int cont = 0;
int cont2 = 0;
int contParam = 0;
int aux;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncEventSource events("/events");


const int sizeOfRecord = 30;
int id;
String values;

int registros;
int registros2;
int registros3;

String NOME[8];
String ANG[8];
int tamanhoSetor = 0;

// Objeto da nossa lib que recebe o nome do arquivo e tamanho fixo de registro
FS_File_Record ObjFS("/teste.bin", sizeOfRecord);
FS_File_Record2 AgFS("/agenda.bin", sizeOfRecord);
FS_File_Record3 PosFS("/agendaPOS.bin", 10);


String dados;
String arqNomes;
String arqAngulos;
int ligaB[10];
int desligaB[10];

// String que recebe as mensagens de erro
String errorMsg;

// Vari??vel que guarda o ??ltimo registro obtido
String lastRecord = "";


// Fun????o reset WD
//void IRAM_ATTR resetModule() {
//  ets_printf("(watchdog) reiniciar\n"); //imprime no log
//  esp_restart(); //reinicia o chip
//}

//void configureWatchdog()
//{
//  timer = timerBegin(0, 80, true); //timerID 0, div 80
//  //timer, callback, interrup????o de borda
//  timerAttachInterrupt(timer, &resetModule, true);
//  //timer, tempo (us), repeti????o
//  timerAlarmWrite(timer, 5 * 1000000, true);
//  timerAlarmEnable(timer); //habilita a interrup????o //enable interrupt
//}


void notifyClients(String text) {
  ws.textAll(text);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "status") == 0) {
      LeEntrada();
      String EstadoWS = "#";

      if (EstadoAtual[0] == '3' || EstadoAtual[0] == '4') {
        if (EstadoAtual[0] == '3') {
          EstadoWS += "Avanco ";
        }
        if (EstadoAtual[0] == '4') {
          EstadoWS += "Reverso ";
        }

      }
      if (EstadoAtual[1] == '5' || EstadoAtual[1] == '6') {
        if (EstadoAtual[1] == '5') {
          EstadoWS += "Seco ";
        }
        if (EstadoAtual[1] == '6') {
          EstadoWS += "Molhado ";
        }

      }
      if (EstadoAtual[2] == '1' || EstadoAtual[2] == '2') {
        if (EstadoAtual[2] == '1') {
          EstadoWS += "Ligado ";
        }
        if (EstadoAtual[2] == '2') {
          EstadoWS = "#Desligado ";
        }
      }

      //Serial2.println(EstadoWS);
      notifyClients(EstadoWS);
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      //Serial2.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      //Serial2.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

//Listagem dos Diretorios
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial2.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial2.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial2.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial2.print("  DIR : ");
      Serial2.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial2.print("  FILE: ");
      Serial2.print(file.name());
      Serial2.print("  SIZE: ");
      Serial2.println(file.size());
    }
    file = root.openNextFile();
  }
}

//task para desarmar os reles caso o painel seja desligado manualmente

void TaskDES( void *pvParameters) {




  long lastTime0 = 0;
  long timer = 1000;
  for (;;) {
    // timerWrite(timer, 0); //ALIMENTA WD

    if ((millis() - lastTime0) > timer) {
      tempo = tempo + (millis() - lastTime0) / 1000;
      lastTime0 = millis();

      //Serial2.println(tempo);

    }

    time_t rawtime = tempo;
    struct tm  ts;
    char       bufT[80];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(bufT, sizeof(bufT), "%H:%M-%S", &ts);
    horaH = String(bufT).substring(0, String(bufT).indexOf(':'));
    horaM = String(bufT).substring(String(bufT).indexOf(':') + 1, String(bufT).indexOf('-'));
    horaS = String(bufT).substring(String(bufT).indexOf('-') + 1, String(bufT).indexOf('\n'));
    horaTS = horaH.toInt() * 3600 + horaM.toInt() * 60 + horaS.toInt();
    strftime(bufT, sizeof(bufT), "%w", &ts);
    hoje = String(bufT).toInt();

    delay(1);
  }
}

void setup()
{

  //Wire.begin(32, 22);// sda // scl
  //rtc.begin();
  //rtc.adjust(DateTime(2022, 2, 25, 14, 50, 00));//Ajusta o tempo do RTC para a data e hora

  time_on = millis();
  Serial2.begin(9600);   //INICIALIZA Serial2., PARA ESP32 LORAV2 NUNCA UTILIZAR A SERIAL1
  //ss.begin(115200);
  //ss.println("INICIOU SOFTWARE SERIAL");
  Serial1.begin(115200, SERIAL_8N1, 21, 13); // rx // tx
  //begin();
  //Serial2.begin(9600);
  //configureWatchdog();

  pinMode(LIGA, OUTPUT);
  pinMode(DESLIGA, OUTPUT);
  pinMode(AVANCO, OUTPUT);
  pinMode(REVERSO, OUTPUT);
  pinMode(MOLHADO, OUTPUT);
  pinMode(RAUX, OUTPUT);
  pinMode(RAUXP, OUTPUT);
  pinMode(PERCAT, OUTPUT);
  pinMode(BOMBA, OUTPUT);

  pinMode(AVREAL, INPUT_PULLUP);
  pinMode(RTREAL, INPUT_PULLUP);
  pinMode(PERC, INPUT_PULLUP);
  pinMode(PRESS, INPUT_PULLUP);

  digitalWrite(LIGA, HIGH);
  digitalWrite(DESLIGA, HIGH);
  digitalWrite(AVANCO, HIGH);
  digitalWrite(REVERSO, HIGH);
  digitalWrite(MOLHADO, HIGH);
  digitalWrite(RAUX, HIGH);
  digitalWrite(RAUXP, HIGH);
  digitalWrite(PERCAT, HIGH);
  digitalWrite(BOMBA, HIGH);

  EstadoAnterior[0] = EstadoAtual[0];
  EstadoAnterior[1] = EstadoAtual[1];
  EstadoAnterior[2] = EstadoAtual[2];
  // Exibe na Serial2."Starting..." para debug
  Serial2.print("Starting...");

  // Se n??o foi poss??vel iniciar o File System, exibimos erro e reiniciamos o ESP
  if (!ObjFS.init())
  {
    Serial2.println("File system error");
    delay(1000);
    //ESP.restart();
  }

  // Exibimos mensagem
  Serial2.println("File system ok");

  // Se o arquivo n??o existe, criamos o arquivo
  if (!ObjFS.fileExists())
  {
    Serial2.println("New file ObjFS");
    ObjFS.newFile(); // Cria o arquivo
  }

  if (!AgFS.fileExists())
  {
    Serial2.println("New file AgFS");
    AgFS.newFile(); // Cria o arquivo
  }
  if (!PosFS.fileExists())
  {
    Serial2.println("New file PosFS");
    PosFS.newFile(); // Cria o arquivo
  }

  //EXECUTA A FUNCAO AGENDAMENTO TODA VEZ QUE INICIA PARA GRAVAR OS VALORES NAS STRINGS DE AGENDAMENTO SALVAS NA FLASH
  Agendamento();
  AgendaPOS();

  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  //INICIALIZA LORA

  if (!LoRa.begin(BAND)) {
    Serial2.println("Starting LoRa failed!");
    while (1);
  }

  //SYNC WORD DEFAUT 0X99, CASO NENHUMA TENHA SIDO SETADA NAS CONFIGURACOES

  //LoRa.setSyncWord(0x99);
  //LoRa.setSpreadingFactor(8);

  //WIFI em modo hibrido STA para desenvolvimento e AP para campo

  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(ssidap, passwordap);
  WiFi.begin(ssid, password);
  int contw = 0;
  while (WiFi.status() != WL_CONNECTED && contw != 8) {

    delay(500);
    //Serial2.println("Connecting to WiFi..");
    contw++;
  }
  initWebSocket();

  //inicializa as variaveis de configura????o salvas na memoria
  inicioH = String(readFile(SPIFFS, "/inicioeco.bin")).substring(0, String(String(readFile(SPIFFS, "/inicioeco.bin"))).indexOf(':')).toInt();
  fimH =    String(readFile(SPIFFS, "/fimeco.bin")).substring(0, String(String(readFile(SPIFFS, "/fimeco.bin"))).indexOf(':')).toInt();
  inicioM = String(readFile(SPIFFS, "/inicioeco.bin")).substring(String(String(readFile(SPIFFS, "/inicioeco.bin"))).indexOf(':') + 1 , String(String(readFile(SPIFFS, "/inicioeco.bin"))).indexOf('\n')).toInt();
  fimM =    String(readFile(SPIFFS, "/fimeco.bin")).substring(String(String(readFile(SPIFFS, "/fimeco.bin"))).indexOf(':') + 1 , String(String(readFile(SPIFFS, "/fimeco.bin"))).indexOf('\n')).toInt();


  TIPO = readFile(SPIFFS, "/contator.txt");
  espera = readFile(SPIFFS, "/delay.txt").toInt() * 1000;
  DelayPressao = readFile(SPIFFS, "/delay_p.txt").toInt() * 60 * 1000;
  nome = readFile(SPIFFS, "/nome.txt");
  LoRaAdress = readFile(SPIFFS, "/sw.txt").toInt();
  if (readFile(SPIFFS, "/modoeco.bin") == "on") {
    eco = "on";
  } else {
    eco = " ";
  }
  arqNomes = readFile(SPIFFS, "/setores.bin");
  arqAngulos = readFile(SPIFFS, "/angulos.bin");
  GPRS_ID = readFile(SPIFFS, "/gprs_ID.txt");
 
  //caso n??o haja nada na memoria ser?? setado o defaut (NA;4s;Pivo 1;sw:0x99)
  if (error == 1) {
    TIPO = "NA";
    writeFile(SPIFFS, "/contator.txt", TIPO.c_str());
    espera = 4000;
    String indelay = String((espera / 1000));
    writeFile(SPIFFS, "/delay.txt", indelay.c_str());
    DelayPressao = 5 * 60 * 1000;
    writeFile(SPIFFS, "/delay_p.txt", String(DelayPressao / 60000).c_str());
    nome = "Pivo 1";
    writeFile(SPIFFS, "/nome.txt", nome.c_str());
    LoRaAdress = 153;
    writeFile(SPIFFS, "/sw.txt", String(LoRaAdress).c_str());
    writeFile(SPIFFS, "/inicioeco.bin", String("18:00").c_str());
    writeFile(SPIFFS, "/fimeco.bin", String("20:00").c_str());
    writeFile(SPIFFS, "/modoeco.bin", eco.c_str());
    arqNomes = "S1_inicio;S1_fim;S2_inicio;S2_fim;S3_inicio;S3_fim;S4_inicio;S4_fim";
    arqAngulos = "30;60;120;150;210;240;300;330";
  
    GPRS_ID = "PIVO_NAO_CADASTRADO";
    
    writeFile(SPIFFS, "/gprs_ID.txt", GPRS_ID.c_str());
  }

  for (int i = 0; i < arqNomes.length(); i++) {
    if (arqNomes[i] == ';' || arqNomes[i] == '\n') {
      tamanhoSetor++;
    }
  }
  if (tamanhoSetor) {
    tamanhoSetor++;
  }
  //Serial2.println(tamanhoSetor);
  for (int i = 0; i < tamanhoSetor / 2; i++) {
    ligaB[i] = getValue(arqAngulos, ';', i * 2).toInt();
    desligaB[i] = getValue(arqAngulos, ';', (i * 2) + 1).toInt();
    //    Serial2.print(ligaB[i]);
    //    Serial2.print("-");
    //    Serial2.println(desligaB[i]);
  }

  //--------------------RELIGAMENTO AUTOMATICO--------------------------------------


  //LoRa.setSyncWord(syncword.toInt());

  Serial2.print("ESP32 IP as soft AP: ");
  Serial2.println(WiFi.softAPIP());

  Serial2.print("ESP32 IP on the WiFi network: ");
  Serial2.println(WiFi.localIP());

  //CODIGO DE GRAVACAO DE ARQUIVOS VIA OTA

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    //Serial2.println("Start updating " + type);
  })
  .onEnd([]() {
    //Serial2.println("nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    //Serial2.printf("Progress: %u%%r", (progress / (total / 100)));
    // timerWrite(timer, 0);
    //digitalWrite(2, !digitalRead(2));
  })
  .onError([](ota_error_t error) {
    Serial2.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial2.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial2.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial2.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial2.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial2.println("End Failed");
  });
  ArduinoOTA.begin();


  //PARAMETRIZACAO DA TASK

  xTaskCreatePinnedToCore(
    TaskDES,
    "TaskDES",
    10000,  // Stack size
    NULL,
    1, // Priority
    NULL,
    0); //CORE 0

  //-------------------------------------------------------------------- WEB SERVER ---------------------------------------------------------------------------------------

  server.on( "/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
  server.on( "/agenda", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/agenda.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/styles.css", "text/css");
  });
  server.on("/logo_irrigabras.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/logo_irrigabras.png");
  });
  server.on("/logo_soil.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/logo_soil.png");
  });
  server.on("/reload.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/reload.png");
  });
  server.on("/play.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/play.png");
  });
  server.on("/rotate_left_small.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/rotate_left_small.png");
  });
  server.on("/rotate_right_small.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/rotate_right_small.png");
  });
  server.on("/stop.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/stop.png");
  });
  server.on("/water_off.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/water_off.png");
  });
  server.on("/water_on.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/water_on.png");
  });


  //AQUISI????O DOS PARAMETROS NA PAGINA WEB
  server.on("/bomba", HTTP_POST, [] (AsyncWebServerRequest * request) {
    int params = request->params();
    contParam = params;
    tamanhoSetor = 0;
    for (int i = 0; i < params; i++) {
      NOME[i] = "";
      ANG[i] = "";
      AsyncWebParameter* p = request->getParam(i);
      int testeAng = 0;
      if (String(p->value().c_str()) == "0") {
        testeAng = 1;
      }
      if (String(p->value().c_str()).toInt() != 0 || testeAng == 1) {
        NOME[i] = String(p->name().c_str());
        ANG[i] = String(p->value().c_str()).toInt();
      }

    }
    Bomba();

    request->send(SPIFFS, "/bomba_injetora.html", String(), false, processor);
  }
           );
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    //------------------------------------------CONFIG------------------------------------------------------------------------

    if (request->hasParam(PARAM_CONTATORA)) {
      TIPO = request->getParam(PARAM_CONTATORA)->value();
      writeFile(SPIFFS, "/contator.txt", TIPO.c_str());
    }
    if (request->hasParam(PARAM_ESPERA)) {

      String indelay = (request->getParam(PARAM_ESPERA)->value());
      espera = indelay.toInt() * 1000;
      writeFile(SPIFFS, "/delay.txt", indelay.c_str());
    }
    if (request->hasParam(PARAM_NOME)) {

      nome = request->getParam(PARAM_NOME)->value();
      writeFile(SPIFFS, "/nome.txt", nome.c_str());
    }
    if (request->hasParam(PARAM_SW)) {

      LoRaAdress = (request->getParam(PARAM_SW)->value()).toInt();
      writeFile(SPIFFS, "/sw.txt", String(LoRaAdress).c_str());
      //LoRa.setSyncWord(syncword.toInt());
      Serial2.println(byte(LoRaAdress));
    }
    if (request->hasParam(PARAM_DelayPressao)) {

      DelayPressao = (request->getParam(PARAM_DelayPressao)->value()).toInt() * 60 * 1000;
      writeFile(SPIFFS, "/delay_p.txt", String(DelayPressao / 60000).c_str());

    }
    if (request->hasParam(PARAM_GPRS_ID)) {
      GPRS_ID = request->getParam(PARAM_GPRS_ID)->value();
      writeFile(SPIFFS, "/gprs_ID.txt", GPRS_ID.c_str());
      StaticJsonDocument<100> reg;
      reg["register"] = "True";
      reg["GPRS_ID"]   = GPRS_ID;
     
      serializeJson(reg, Serial1);
    }
    if (request->hasParam(PARAM_NODE_ID)) {
      //NODE_ID = request->getParam(PARAM_NODE_ID)->value();
      //writeFile(SPIFFS, "/node_ID.txt", NODE_ID.c_str());
      StaticJsonDocument<100> reg;
      reg["register"] = "True";
      reg["GPRS_ID"]   = GPRS_ID;
      //reg["NODE_ID"]   = NODE_ID;
      serializeJson(reg, Serial1);
    }
    //-----------------------------------------------------------------------------------------------------------------------
    if (request->hasParam("modoeco")) {
      if (request->getParam("modoeco")->value() == "on") {
        eco = request->getParam("modoeco")->value();
      } else {
        eco = " ";
      }
      writeFile(SPIFFS, "/modoeco.bin", eco.c_str());
      request->send(SPIFFS, "/index.html", String(), false, processor);
    }
    if (request->hasParam("inicioeco") && request->hasParam("fimeco")) {
      String inicio = request->getParam("inicioeco")->value();
      String fim = request->getParam("fimeco")->value();
      if (request->hasParam("switch")) {
        eco = request->getParam("switch")->value();
      } else {
        eco = " ";
      }
      writeFile(SPIFFS, "/modoeco.bin", eco.c_str());

      writeFile(SPIFFS, "/inicioeco.bin", inicio.c_str());
      writeFile(SPIFFS, "/fimeco.bin", fim.c_str());
      inicioH = String(readFile(SPIFFS, "/inicioeco.bin")).substring(0, String(String(readFile(SPIFFS, "/inicioeco.bin"))).indexOf(':')).toInt();
      fimH =    String(readFile(SPIFFS, "/fimeco.bin")).substring(0, String(String(readFile(SPIFFS, "/fimeco.bin"))).indexOf(':')).toInt();
      inicioM = String(readFile(SPIFFS, "/inicioeco.bin")).substring(String(String(readFile(SPIFFS, "/inicioeco.bin"))).indexOf(':') + 1 , String(String(readFile(SPIFFS, "/inicioeco.bin"))).indexOf('\n')).toInt();
      fimM =    String(readFile(SPIFFS, "/fimeco.bin")).substring(String(String(readFile(SPIFFS, "/fimeco.bin"))).indexOf(':') + 1 , String(String(readFile(SPIFFS, "/fimeco.bin"))).indexOf('\n')).toInt();



      //Serial2.println(readFile(SPIFFS, "/inicioeco.bin"));

      //Serial2.println(readFile(SPIFFS, "/fimeco.bin"));
      request->send(SPIFFS, "/eco.html", String(), false, processor);
    }

    //---------------------------------------AGENDAMENTO POR HORARIO---------------------------------------------------------

    if (request->hasParam(PARAM_INPUT_2_1)) {

      //String Sai = String(request->getParam(PARAM_INPUT_2_1)->value() + " " + request->getParam(PARAM_INPUT_2_2)->value() + " " + request->getParam(PARAM_INPUT_2_3)->value() + " " + request->getParam(PARAM_INPUT_2_4)->value());
      if (request->getParam(PARAM_INPUT_2_1)->value() == "2") {
        String horarioag = request->getParam(PARAM_INPUT_2_4)->value();
        String ano = (horarioag.substring(0, 4));
        String mes = (horarioag.substring(5, 7));
        String dia = (horarioag.substring(8, 10));
        String hora = (horarioag.substring(11, 16));
        horarioag = String(hora + " - " + dia + "/" + mes + "/" + ano);
        String Sai = String("002/0-" + horarioag);

        values = Sai;

        while (values.length() < 30) {
          values += " ";
        }

        if (registros2 < 4) {
          if (values != "" && !AgFS.writeFile(values, &errorMsg))
            Serial2.println(errorMsg);
        }


        values = "";
        Agendamento();


      }
      if (request->getParam(PARAM_INPUT_2_1)->value() == "1") {
        String perc_a = request->getParam(PARAM_PERC_H)->value();
        //num = perc_a.toInt();
        //Serial2.println(num);
        String horarioag = request->getParam(PARAM_INPUT_2_4)->value();
        String ano = (horarioag.substring(0, 4));
        String mes = (horarioag.substring(5, 7));
        String dia = (horarioag.substring(8, 10));
        String hora = (horarioag.substring(11, 16));
        horarioag = String(hora + " - " + dia + "/" + mes + "/" + ano);
        String Sai = String(request->getParam(PARAM_INPUT_2_2)->value() + request->getParam(PARAM_INPUT_2_3)->value() + request->getParam(PARAM_INPUT_2_1)->value() + "/" + perc_a + "-" + horarioag);
        Serial2.println(Sai);
        values = Sai;

        while (values.length() < 30) {
          values += " ";
        }

        if (registros2 < 4) {
          if (values != "" && !AgFS.writeFile(values, &errorMsg))
            Serial2.println(errorMsg);
        }

        values = "";
        Agendamento();
        if (request->getParam(PARAM_INPUT_2_5)->value() != "") {
          String horarioag = request->getParam(PARAM_INPUT_2_5)->value();
          String ano = (horarioag.substring(0, 4));
          String mes = (horarioag.substring(5, 7));
          String dia = (horarioag.substring(8, 10));
          String hora = (horarioag.substring(11, 16));
          horarioag = String(hora + " - " + dia + "/" + mes + "/" + ano);
          String Sai = String("002/0-" + horarioag);
          Serial2.println(Sai);
          values = Sai;

          while (values.length() < 30) {
            values += " ";
          }
          //Serial2.println(values);
          if (registros2 < 4) {
            if (values != "" && !AgFS.writeFile(values, &errorMsg))
              Serial2.println(errorMsg);
          }
          //showFile2();
          values = "";
          Agendamento();
        }
      }
      ws.textAll("%");
      request->send(SPIFFS, "/agendaH.html", String(), false, processor);
    }

    //-----------------------------------------------------------------------------------------------------------------------

    //---------------------------------------AGENDAMENTO POR POSI????O---------------------------------------------------------

    if (request->hasParam(PARAM_INPUT_3_1)) {
      if (request->getParam(PARAM_INPUT_3_1)->value() == "1") {
        String angW = request->getParam(PARAM_INPUT_3_3)->value();
        String acaoPOS;
        acaoPOS = "002";
        Serial2.println(acaoPOS + "-" + angW);
        values = String(acaoPOS + "-" + angW + "/0");

        while (values.length() < 10) {
          values += " ";
        }

        if (values != "" && !PosFS.writeFile(values, &errorMsg))
          Serial2.println(errorMsg);

        values = "";
        RETflag = 1;
        AgendaPOS();

      }
      if (request->getParam(PARAM_INPUT_3_1)->value() == "2") {
        String angW = request->getParam(PARAM_INPUT_3_3)->value();
        String acaoPOS;
        acaoPOS = "RET";
        //        if (request->hasParam("percRET")) {
        //          percRET = request->getParam("percRET")->value().toInt();
        //        } else {
        //          percRET = 100;
        //        }
        percRET = 100;

        values = String(acaoPOS + "-" + angW + "/" + percRET);

        while (values.length() < 10) {
          values += " ";
        }

        if (values != "" && !PosFS.writeFile(values, &errorMsg))
          Serial2.println(errorMsg);

        values = "";
        angW = request->getParam(PARAM_INPUT_3_4)->value();
        //Serial2.println("RET-" + angW);
        values = String("002-" + angW + "/0");

        while (values.length() < 10) {
          values += " ";
        }

        if (values != "" && !PosFS.writeFile(values, &errorMsg))
          Serial2.println(errorMsg);

        values = "";
        AgendaPOS();
      }
      if (request->getParam(PARAM_INPUT_3_1)->value() == "3") {
        String angW = request->getParam(PARAM_INPUT_3_3)->value();
        String acaoPOS;
        acaoPOS = "RETi";
        if (request->hasParam("percRET")) {
          percRET = request->getParam("percRET")->value().toInt();
        } else {
          percRET = 100;
        }


        values = String(acaoPOS + "-" + angW  + "/" + percRET);

        while (values.length() < 10) {
          values += " ";
        }

        if (values != "" && !PosFS.writeFile(values, &errorMsg))
          Serial2.println(errorMsg);

        values = "";
        angW = request->getParam(PARAM_INPUT_3_4)->value();
        //Serial2.println("RET-" + angW);
        values = String("002-" + angW + "/0");

        while (values.length() < 10) {
          values += " ";
        }

        if (values != "" && !PosFS.writeFile(values, &errorMsg))
          Serial2.println(errorMsg);

        values = "";
        AgendaPOS();
      }
      ws.textAll("%");
      request->send(SPIFFS, "/AgendaP.html", String(), false, processor);
    }
    //-----------------------------------------------------------------------------------------------------------------------

    //------------------------------------------------ ATUA????O --------------------------------------------------------------

    if (request->hasParam(PARAM_INPUT_0)) {
      if (request->getParam(PARAM_INPUT_0)->value() == "1") {
        INWEB[2] = '1';

        if (request->hasParam(PARAM_INPUT_0_2)) {
          if (request->getParam(PARAM_INPUT_0_2)->value() == "3") {
            INWEB[0] = '3';
          }
          if (request->getParam(PARAM_INPUT_0_2)->value() == "4") {
            INWEB[0] = '4';
          }
        }
        if (request->hasParam(PARAM_INPUT_0_3)) {
          if (request->getParam(PARAM_INPUT_0_3)->value() == "5") {
            INWEB[1] = '5';
          }
          if (request->getParam(PARAM_INPUT_0_3)->value() == "6") {
            INWEB[1] = '6';
          }

        }
        if (request->hasParam(PARAM_INPUT_1)) {
          inputMessage = request->getParam(PARAM_INPUT_1)->value();
          if (percs != inputMessage) {
            percs = inputMessage;
            numw = percs.toInt();
            aux2 = 0;
          }
        }

        webflag = 1;
      } else {
        if (request->getParam(PARAM_INPUT_0)->value() == "2") {
          INWEB[0] = '0';
          INWEB[1] = '0';
          INWEB[2] = '2';
          inputMessage = "000";

          webflag = 1;
        }
      }
      request->send(SPIFFS, "/index.html", String(), false, processor);
    }




  });

  //------------------------------------------------ROTAS------------------------------------------------

  server.on("/bomba", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/bomba_injetora.html", String(), false, processor);
  });
  server.on("/agen", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/agenda.html", String(), false, processor);
  });
  server.on("/eco", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/eco.html", String(), false, processor);
  });
  server.on("/xama", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(String(EstadoAtual[0]) + "-" +  String(EstadoAtual[1]) + "-" + String(EstadoAtual[2]) + "-" + String(perc) + "-" + String(angulo.toInt()) + "-" + String(tempo)));
  });
  server.on("/xamaeco", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(String(readFile(SPIFFS, "/inicioeco.bin")) + "-" + String(readFile(SPIFFS, "/fimeco.bin")) + "-" + String(auxeco) + "-" + String(eco == "on" ? "on" : "off")));
  });
  server.on("/aghora", HTTP_GET, [](AsyncWebServerRequest * request) {
    //    showFile2();
    //    //request->send(200, "text/plain", String(dados));
    //    request->send(SPIFFS, "/agenda.bin" , "text/plain");

    StaticJsonDocument<400> doc;
    JsonArray data = doc.createNestedArray("Atua");
    JsonArray data1 = doc.createNestedArray("Hora");
    JsonArray data2 = doc.createNestedArray("Percent");
    for (int i = 0; i < registros2; i++) {
      data.add(atuaag[i]);
      time_t rawtime = horaag[i];
      struct tm  ts;
      char       buf[80];

      // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
      ts = *localtime(&rawtime);
      strftime(buf, sizeof(buf), "%H:%M:%S %d-%m-%Y", &ts);
      data1.add(buf);
      data2.add(percag[i]);
    }
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);

  });
  server.on("/agpos", HTTP_GET, [](AsyncWebServerRequest * request) {
    StaticJsonDocument<200> doc;
    JsonArray data = doc.createNestedArray("Atua");
    JsonArray data1 = doc.createNestedArray("POS");
    JsonArray data2 = doc.createNestedArray("PERC");
    for (int i = 0; i < registros3; i++) {
      data.add(atuaP[i]);
      data1.add(pos[i]);
      data2.add(percA[i]);
    }
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  server.on("/ecodata", HTTP_GET, [](AsyncWebServerRequest * request) {
    StaticJsonDocument<200> doc;
    JsonArray data = doc.createNestedArray("Eco");
    JsonArray data1 = doc.createNestedArray("Inicio");
    JsonArray data2 = doc.createNestedArray("Fim");

    data.add(eco);
    data1.add(readFile(SPIFFS, "/inicioeco.bin"));
    data2.add(readFile(SPIFFS, "/fimeco.bin"));
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  server.on("/hist", HTTP_GET, [](AsyncWebServerRequest * request) {
    StaticJsonDocument<2048> dochist;
    JsonArray intencaoJSON = dochist.createNestedArray("Intencao");
    JsonArray tsJSON = dochist.createNestedArray("Timestamp");

    dados = "";
    int count = 0;
    String linha = "";
    String linhastr = "";
    int tamanho = 0;
    registros = 0;
    // Exibe na Serial2.e no display o in??cio do arquivo
    //Serial2.println("# Begin of file 1 #");
    errorMsg = "";

    // Posiciona o ponteiro do arquivo no in??cio
    ObjFS.rewind();

    // Exibe todos os registros at?? o fim
    while (ObjFS.readFileNextRecord(&linha, &errorMsg) && linha != "")
    {
      dados += linha;
      intencaoJSON.add(linha.substring(linha.indexOf(":") + 1, linha.indexOf("/")));
      tsJSON.add(linha.substring(linha.indexOf("/") + 1, linha.indexOf(" ")));
      //Serial2.println(linhastr);
      tamanho += linhastr.length();
      //  Serial2.println(linhastr.length());
      //Serial2.println(linha);
      count++;

    }

    // Se existir mensagem de erro exibe na Serial2.e no display
    if (errorMsg != "")
    {
      //Serial2.println(errorMsg);
    }

    // Exibe na Serial2.e no display o fim do arquivo
    //Serial2.println("# End of file 1#");
    registros = count;
    //DynamicJsonDocument<tamanho> dochist;
    String response;
    int b = serializeJson(dochist, response);
    request->send(200, "application/json", response);
    Serial2.println(b);
    // request->send(200, "application/json", responseHist);
  });
  server.on("/agendaP", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/AgendaP.html", String(), false, processor);
  });
  server.on("/agendaH", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/agendaH.html", String(), false, processor);
  });

  //  server.on("/get-message", HTTP_GET, [](AsyncWebServerRequest * request) {
  //    StaticJsonDocument<400> doc;
  //    JsonArray data = doc.createNestedArray("Radios");
  //    data.add(1);
  //    data.add(2);
  //
  //    JsonArray data1 = doc.createNestedArray("rssi");
  //    data1.add(-50);
  //    data1.add(-78);
  //
  //    JsonArray data2 = doc.createNestedArray("rota");
  //    data2.add("1>C");
  //    data2.add("2>1>C");
  //    String response;
  //    serializeJson(doc, response);
  //    request->send(200, "application/json", response);
  //  });

  //BOTAO PARA APAGAR AGENDAMENTOS

  server.on("/clearag", HTTP_GET, [](AsyncWebServerRequest * request) {
    AgFS.destroyFile();
    PosFS.destroyFile();
    ws.textAll("%");
    for (int i = 0; i < 4; i++) {
      horaag[i] = 0;
      atuaag[i] = " ";
    }
    Serial2.println("Delete ag OK");
    request->send(SPIFFS, "/agenda.html", String(), false, processor);
  });


  // Start server
  events.onConnect([](AsyncEventSourceClient * client) {
    if (client->lastId()) {
      Serial2.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();

  countFile();
  String reg = ObjFS.findRecord(registros - 1);
  int idc = reg.indexOf(':');
  String ids = reg.substring(0, idc);
  id = ids.toInt();
  //Serial2.println(readFile(SPIFFS, "/estadoanteriorAUTO.bin"));
  LeEntrada();
}

// Exibe o espa??o total, usado e dispon??vel na memoria
void showAvailableSpace()
{
  Serial2.println("Space: " + String(ObjFS.getTotalSpace()) + " Bytes");
  Serial2.println("Used: " + String(ObjFS.getUsedSpace()) + " Bytes");

}

void loop()
{


  if (Serial1.available()) {
    String mqttmsg = Serial1.readString();
    //Serial1.print("Serial1 - 0");
    //Serial1.print(mqttmsg);
    StaticJsonDocument<200> GPRS;
    deserializeJson(GPRS, mqttmsg);
    if (GPRS["register"] == "True") { //Se enviar register: true, registra um novo GPRS_ID
      StaticJsonDocument<100> reg;
      reg["register"] = "True";
      reg["GPRS_ID"]   = GPRS_ID;
      serializeJson(reg, Serial1);
    } else {  // Caso contr??rio, extrai os numeros do payload
      String payload = GPRS["payload"];
      for (int i = 0; i < payload.length(); i++) {
        stats[i] = payload[i];
      }
      stats[payload.length()] = '#';
      //Serial2.println(stats);
      tempo = GPRS["timestamp"];
      //Serial2.println(tempo);
      mqttFlag = 1;
    }
  }

  //------------------------------------ Religamento Autom??tico ----------------------------------------
  
    if (((millis() - time_on) > DelayReliga) && auxRA == 0)
    {
      String RA = readFile(SPIFFS, "/estadoanteriorAUTO.bin");
      stats[0] = RA[0];
      stats[1] = RA[2];
      stats[2] = RA[4];
      numw = RA.substring(6, RA.indexOf('\n')).toInt();
      EnviaEstado();
      auxRA = 1;
    }
  //--------------------------------------------------------------------------------------------------------

int iniTS = inicioH * 3600 + inicioM * 60;
  int fimTS = fimH * 3600 + fimM * 60;
  //int horaTS = horaH.toInt() * 3600 + horaM.toInt() * 60 + horaS.toInt();


  if (eco == "on" && (iniTS <= horaTS && horaTS < fimTS + 15) && hoje != 0 && hoje != 6) {

    if (auxeco == 0) {
      //atuacao do modo economia aqui    \/
      //writeFile(SPIFFS, "/estadoanterior.bin", String(String(EstadoAtual[0]) + "-" +  String(EstadoAtual[1]) + "-" + String(EstadoAtual[2]) + "-" + String(perc)).c_str());
      //Serial2.println(readFile(SPIFFS, "/estadoanterior.bin"));
      notifyClients("&");
      if (TIPO == "NA" && (digitalRead(AVREAL) == LOW || digitalRead(RTREAL) == LOW)) {
        DesligaEco();
      }
      if (TIPO == "NF" && (digitalRead(AVREAL) == HIGH || digitalRead(RTREAL) == HIGH)) {
        DesligaEco();
      }
      //----------------------------------
      auxeco = 1;
      Serial2.println("MODO ECO");
    }

  } else {
    if (auxeco == 1) {
      Serial2.println("Sai do Eco");
      notifyClients("&");;
      auxeco = 0;
      String estadoanterior = readFile(SPIFFS, "/estadoanterior.bin");
      //Serial2.println(estadoanterior);
      INWEB[0] = estadoanterior[0];
      INWEB[1] = estadoanterior[2];
      INWEB[2] = estadoanterior[4];
      numw = estadoanterior.substring(6, estadoanterior.indexOf('\n')).toInt();
      webflag = 1;
      //Serial2.println(numw);
      delay(2000);

    }

  }
  tatual = millis();


  //timerWrite(timer, 0);
  ArduinoOTA.handle();

  // RECEBE E TRATA O PACOTE LORA

  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    while (LoRa.available()) {
      int LoRaAdressR = LoRa.read();
      //Serial2.println(LoRaAdressR);
      if (LoRaAdressR == LoRaAdress) {
        LoRaData = LoRa.readString();
        rssi = LoRa.packetRssi();
        int separa = LoRaData.indexOf('-');
        int fim = LoRaData.indexOf('#');
        if (separa >= 0) {
          angulo = LoRaData.substring(0, separa);
          hora = LoRaData.substring((separa + 1), (fim));
          if ((LoRaData.substring((LoRaData.indexOf('#') + 1), LoRaData.indexOf('\n')).toInt()) > 1631533539) {
            tempo = (LoRaData.substring((LoRaData.indexOf('#') + 1), LoRaData.indexOf('\n'))).toInt();
          }
        }
      } else {
        String msgerrada = LoRa.readString();
        //Serial2.println("MSG ERRADA");
      }
    }
  }

  if (TIPO == "NF") {
    if (digitalRead(AVREAL) == LOW && digitalRead(RTREAL) == LOW && flagMolhado == 0) {
      //      timerWrite(timer, 0);
      cont++;
      delay(100);
      if (cont > 50) {
        digitalWrite(LIGA, HIGH);
        digitalWrite(DESLIGA, HIGH);
        digitalWrite(AVANCO, HIGH);
        digitalWrite(REVERSO, HIGH);
        digitalWrite(MOLHADO, HIGH);
        digitalWrite(RAUX, HIGH);
        digitalWrite(RAUXP, HIGH);
        digitalWrite(PERCAT, HIGH);
        digitalWrite(BOMBA, HIGH);
        cont = 0;
        perc = 0;
        auxP = 0;
        num = 0;
        aux2 = 0;
      }
    } else {
      cont = 0;
    }
  }

  if (TIPO == "NA") {
    if (digitalRead(AVREAL) == HIGH && digitalRead(RTREAL) == HIGH && flagMolhado == 0) {
      //      timerWrite(timer, 0);
      cont++;
      delay(100);
      if (cont > 50) {
        digitalWrite(LIGA, HIGH);
        digitalWrite(DESLIGA, HIGH);
        digitalWrite(AVANCO, HIGH);
        digitalWrite(REVERSO, HIGH);
        digitalWrite(MOLHADO, HIGH);
        digitalWrite(RAUX, HIGH);
        digitalWrite(RAUXP, HIGH);
        digitalWrite(PERCAT, HIGH);
        digitalWrite(BOMBA, HIGH);
        cont = 0;
        perc = 0;
        auxP = 0;
        num = 0;
        aux2 = 0;
      }
    } else {
      cont = 0;
    }
  }
  //-------------------------CONTADOR MOLHADO-----------------------------------------------------
  if (TIPO == "NF") {
    if (digitalRead(AVREAL) == LOW && digitalRead(RTREAL) == LOW && flagMolhado == 1) {
      //      timerWrite(timer, 0);
      cont2++;
      delay(100);
      if (cont2 > DelayPressao / 100) {
        digitalWrite(LIGA, HIGH);
        digitalWrite(DESLIGA, HIGH);
        digitalWrite(AVANCO, HIGH);
        digitalWrite(REVERSO, HIGH);
        digitalWrite(MOLHADO, HIGH);
        digitalWrite(RAUX, HIGH);
        digitalWrite(RAUXP, HIGH);
        digitalWrite(PERCAT, HIGH);
        digitalWrite(BOMBA, HIGH);
        cont2 = 0;
        perc = 0;
        auxP = 0;
        num = 0;
        aux2 = 0;
      }
    } else {
      cont2 = 0;
    }
  }
  if (flagMolhado == 1 && digitalRead(PRESS) == LOW) {
    flagMolhado = 0;
    digitalWrite(BOMBA, HIGH);
  }

  if (TIPO == "NA") {
    if (digitalRead(AVREAL) == HIGH && digitalRead(RTREAL) == HIGH && flagMolhado == 1) {
      //      timerWrite(timer, 0);
      cont2++;
      delay(100);
      if (cont2 > DelayPressao / 100) {
        digitalWrite(LIGA, HIGH);
        digitalWrite(DESLIGA, HIGH);
        digitalWrite(AVANCO, HIGH);
        digitalWrite(REVERSO, HIGH);
        digitalWrite(MOLHADO, HIGH);
        digitalWrite(RAUX, HIGH);
        digitalWrite(RAUXP, HIGH);
        digitalWrite(PERCAT, HIGH);
        digitalWrite(BOMBA, HIGH);
        cont2 = 0;
        perc = 0;
        auxP = 0;
        num = 0;
        aux2 = 0;
      }
    } else {
      cont2 = 0;
    }
  }
  //----------------------------------------------------------------------------------------------
  if ( Serial2.available() > 0 || webflag == 1 || mqttFlag == 1) { //se o Serial2.receber uma mensagem de 6 caracteres ou receber uma msg do WebServer
    
    int buffersize =  Serial2.available();
    //Serial2.println("COASda");

    delay(1000);
    //epoch = epoch +1;
    if (mqttFlag != 1) {
      for (int i = 0; i < 50; i++) {
        if ( Serial2.available()) {
        //Serial.print("Serial2 - 0");
          
          stats[i] =  Serial2.read();
          //Serial1.print(stats[i]);
        } else {
          stats[i] = '#';
          i = 51;
        }
      }
    }
    //Serial2.print(stats);
    if ((stats[0] == '3' || stats[0] == '4')  && webflag == 0) {
      Snum = String((stats[3] - '0') * 100 + (stats[4] - '0') * 10 + (stats[5] - '0'));
      num = Snum.toInt();
      aux2 = 0;
      buffersize = 6;
    }
    if (stats[2] == '2') {
      buffersize = 6;
    }
    if (webflag == 1) {
      stats[0] = INWEB[0];
      stats[1] = INWEB[1];
      stats[2] = INWEB[2];
    }


    //Serial2.println("OI");
    //Serial2.println(stats);
    if (buffersize == 6 || webflag == 1) {

      if (stats[0] == '0' && stats[1] == '0' && stats[2] == '0') {
        LeEntrada();
        EnviaStatus();
        for (int i = 0; i <= buffersize; i++) {
          stats[i] = ' ';
        }
      }

      if ( stats[0] != '0' || stats[1] != '0' || stats[2] != '0') {

        EnviaEstado();
        LeEntrada();
        EnviaStatus();
        for (int i = 0; i <= buffersize; i++) {
          stats[i] = ' ';
        }
      }
      webflag = 0;
      return;
    } else
    {
      if (stats[0] == '0' && stats[1] == '0' && stats[2] == '0') {
        LeEntrada();
        EnviaStatus();
      }

      //DESLIGA

      if (stats[0] == '0' && stats[1] == '0' && stats[2] == '2') {
        Desliga();
        LeEntrada();
        EnviaStatus();
      }

      if ((stats[0] == 'G' && stats[1] == 'P' && stats[2] == 'S') || (stats[1] == 'G' && stats[2] == 'P' && stats[3] == 'S') || (stats[2] == 'G' && stats[3] == 'P' && stats[4] == 'S')) {
        String GPSdata = String(stats).substring(String(stats).indexOf('S') + 1, String(stats).indexOf('$'));
        //Serial2.print(GPSdata);
        int separa = GPSdata.indexOf('-');
        int fim = GPSdata.indexOf('#');
        if (separa >= 0) {
          angulo = GPSdata.substring(0, separa);
          hora = GPSdata.substring((separa + 1), (fim));
          if ((GPSdata.substring((GPSdata.indexOf('#') + 1), GPSdata.indexOf('$')).toInt()) > 1631533539) {
            tempo = (GPSdata.substring((GPSdata.indexOf('#') + 1), GPSdata.indexOf('$'))).toInt();
          }
        }
      }


      if (stats[0] == 'R' && stats[1] == 'S' && stats[2] == 'T') {
        Serial2.println("Reiniciando");
        delay(500);
        ESP.restart();
      }
      if (stats[0] == 'D' && stats[1] == 'E' && stats[2] == 'L') {
        ObjFS.destroyFile();
        Serial2.println("Delete OK");
        id = 0;
        return;
      }
      if (stats[0] == 'S' && stats[1] == 'F') {
        showFile();
        Serial2.print("Numero de Registros: ");
        Serial2.println(registros);
        showAvailableSpace();
        return;
      }
      if (stats[0] == 'R' && stats[1] == 'N') {
        String statss = String(stats);
        statss = statss.substring((statss.indexOf('N') + 1), statss.indexOf('#'));
        int busca = statss.toInt() - 1;
        for (int i = (busca); i < id; i++) {
          int busca = statss.toInt() - 1;
          String reg = ObjFS.findRecord(i);
          Serial2.println(reg);
          //delay(100);
        }
        return;
      }
      if (stats[0] == 'S' && stats[1] == 'T' && stats[2] == 'R') {
        showFile();
        Serial2.println("Historico:");
        Serial2.println(dados);
        return;
      }
      if (stats[0] == 'L' && stats[1] == 'S' && stats[2] == 'T') {
        Serial2.println("teste LST");
        String reg = ObjFS.findRecord(id - 1);
        Serial2.println(reg);
        return;
      }
      if (stats[0] == 'D' && stats[1] == 'I' && stats[2] == 'R') {
        listDir(SPIFFS, "/", 0);
        showAvailableSpace();
        return;
      }

      //******************************************************************agendamento*****************************************************************************************

      if (stats[0] == 'A' && stats[1] == 'G') {
        //Serial2.print(String(stats));
        values = String(stats).substring((String(stats).indexOf('>') + 1), (String(stats).indexOf('#')));

        while (values.length() < 30) {
          values += " ";
        }
        //Serial2.println(values);
        if (values != "" && !AgFS.writeFile(values, &errorMsg))
          Serial2.println(errorMsg);
        values = "";
        Agendamento();

      }
      if (stats[0] == 'S' && stats[1] == 'A') {
        showFile2();
        //        Serial2.print("Numero de Registros: ");
        //        Serial2.println(registros2);
        //        showAvailableSpace();
        Serial2.println("Agendamentos por horario:");
        Serial2.print(dados);
        return;
      }
      if (stats[0] == 'S' && stats[1] == 'P') {
        showFile3();
        Serial2.print("Numero de Registros: ");
        Serial2.println(registros3);
        showAvailableSpace();
        return;
      }
      if (stats[0] == 'A' && stats[1] == 'D' && stats[2] == 'E' && stats[3] == 'L') {
        AgFS.destroyFile();
        PosFS.destroyFile();
        Serial2.println("Delete ag OK");
        //id = 0;
        return;
        }
                         /*  Lista de comandos que podem serem enviados via R??dios
                               SF: exibe a lista inteira
                               RN(numero): exibe a partir de um registro especifico
                               LST: exibe o ultimo registro
                               DEL: deleta a lista inteira
                               ADEL: deleta os agendamentos
                               STR: Salva a lista inteira em uma string "dados"
                               RST: Reset via serial
                               SA: exibe agendamentos por horario
                               SP: exibe agendamentos por posi????o
  
                               NENHUM COMANDO PODE TER 6 CARACTERES
                          */
    }

    mqttFlag = 0;
  } else {
    Serial2.read();
    delay(10);
  }
  if ((millis() - UltimaLeitura) > DelayLeitura) {
    LeEntrada();
    UltimaLeitura = millis();
  }


  //Envia informa????es para pagina web via /events
  if ((millis() - lastTime) > timerDelay) {
    // epoch = epoch + 5;
    //Serial2.println(hoje);
    //notifyClients();
    ws.cleanupClients();
    //    String eventoEstado = String(String(sentido) + " " + String(seco) + " " + String(ligado));
    //    events.send(String(sentido).c_str(), "sentido", millis());
    //    events.send(String(seco).c_str(), "seco", millis());
    //    events.send(String(ligado).c_str(), "ligado", millis());
    events.send(String(perc).c_str(), "STRperc", millis());
    Agendamento2();
    StaticJsonDocument<400> doc;
    JsonArray data = doc.createNestedArray("Atua");
    JsonArray data1 = doc.createNestedArray("Hora");
    JsonArray data2 = doc.createNestedArray("Percent");
    for (int i = 0; i < registros2; i++) {
      String intencao;
      if (atuaag2[i] == "351") {
        intencao = "Avan??o Seco Ligado";
      }
      if (atuaag2[i] == "451") {
        intencao = "Reverso Seco Ligado";
      }
      if (atuaag2[i] == "361") {
        intencao = "Avan??o Molhado Ligado";
      }
      if (atuaag2[i] == "461") {
        intencao = "Reverso Molhado Ligado";
      }
      if (atuaag2[i] == "002") {
        intencao = "Desligado";
      }
      data.add(intencao);
      time_t rawtime = horaag2[i];
      struct tm  ts;
      char       buf[80];

      // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
      ts = *localtime(&rawtime);
      strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
      data1.add(buf);
      data2.add(percag[i]);
    }
    String JSONhora;
    serializeJson(doc, JSONhora);
    events.send(String(JSONhora).c_str(), "JSONhora", millis());

    AgendaPOS2();
    StaticJsonDocument<200> doc2;
    JsonArray datap = doc2.createNestedArray("Atua");
    JsonArray data1p = doc2.createNestedArray("POS");

    for (int i = 0; i < registros3; i++) {
      String intencao;
      if (atuaP2[i] == "RET") {
        intencao = "Retorno";
      }
      if (atuaP2[i] == "RETi") {
        intencao = "Retorno Irrigando";
      }
      if (atuaP2[i] == "002") {
        intencao = "Desliga";
      }
      datap.add(intencao);
      data1p.add(pos2[i]);

    }
    String JSpos;
    serializeJson(doc2, JSpos);
    events.send(String(JSpos).c_str(), "JSpos", millis());


    events.send(String(angulo.toInt()).c_str(), "angulo", millis());
    //events.send(String(rssi).c_str(), "rssi", millis());
    lastTime = millis();
    //
    //    Serial2.println(iniTS);
    //    Serial2.println(fimTS);
    //    Serial2.println(horaTS);
  }
  endloop = millis() - tatual;
  Percentimetro();
  AtuaPercentimetro();
  AtuaAg();
  AtuaPOS();
  LigaBomba();
  
//
//  static uint8_t tempoPass = 10;
//  if(tempoPass > 0)
//  {
//    tempoPass--;
//  }
//  else
//  {
//    digitalWrite(BOMBA,!digitalRead(BOMBA));
//    tempoPass = 10;
//  }
  

}

//Fun????o para leitura de arquivos de configura????o

String readFile(fs::FS &fs, const char * path) {
  //Serial2.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    Serial2.println("- empty file or failed to open file");
    error = 1;
    return String();
  }
  //Serial2.println("- read from file:");
  String fileContent;
  while (file.available()) {
    fileContent += String((char)file.read());
  }
  file.close();
  error = 0;
  return fileContent;
}

//Fun????o para grava????o de arquivos de configura????o

void writeFile(fs::FS &fs, const char * path, const char * message) {
  //Serial2.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file) {
    Serial2.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    //Serial2.println("- file written");
  } else {
    Serial2.println("- write failed");
  }
  file.close();
}
