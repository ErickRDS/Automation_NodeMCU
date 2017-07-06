#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <IRremoteESP8266.h>

//CÓDIGO RAW DOS BOTÕES
IRsend irsend(14); //D5
//VOLTAR
unsigned int VOLTAR[38] = {2650,1050,400,1050,450,600,400,600,1300,1500,400,600,400,550,450,550,450,550,450,550,400,550,450,600,400,550,400,550,450,550,400,600,850,1000,850,1050,400}; 
//MENU
unsigned int MENU[38] = {2650,950,450,1000,450,600,450,600,400,1000,850,550,450,600,450,550,400,550,450,550,450,550,450,550,450,600,450,550,850,1000,850,1000,850,1000,400,600,450};
//MUTE
unsigned int MUTE[38] = {2700,1000,450,1050,450,600,450,550,1300,1500,450,550,450,550,450,500,500,600,450,550,450,550,450,550,450,550,450,600,450,600,450,550,900,550,450,1000,850};
//Vol- (TV)
unsigned int VMENOS[38] = {2600,1000,450,1000,450,550,450,550,1300,1500,400,600,400,550,450,550,400,550,450,550,400,550,450,550,450,550,400,550,450,550,850,1000,450,550,450,550,850};
//Vol+ (TV)
unsigned int VMAIS[38] = {2650,1000,450,1050,400,600,400,600,1300,1500,450,550,450,550,450,550,450,550,450,550,450,600,400,600,400,600,400,600,400,600,850,1050,400,600,450,550,900};

int tamanho = 38; //TAMANHO DA LINHA RAW(38 BLOCOS)
int frequencia = 32; //FREQUÊNCIA DO SINAL IR(32KHz)

//DEFININDO O TEMPO DE PROCESSAMENTO
unsigned long tic;
unsigned long toc;

//DEFININDO A REDE
const char* ssid = "Lopes 2.4GHz";                      //Nome da sua rede Wifi
const char* password = "naovoutedizer";                //Senha da rede

//CONFIGURANDO A REDE
IPAddress ip(192, 168, 1, 70);                     //IP do ESP (para voce acessar pelo browser)
IPAddress gateway(192, 168, 1, 1);                 //IP do roteador da sua rede wifi
IPAddress subnet(255, 255, 255, 0);                 //Mascara de rede da sua rede wifi
WiFiServer server(80);                              //Criando o servidor web na porta 80

//Configurando Função do Sensor TEMPERATURA - D1-(5)
#define DHTPIN 5                                    //D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Sensor Pino D0 => Sensor de PRESENÇA D0=16
int switchValue;
int switchPin = 4;
String senspren = "Inativo"; //Deixar a string em aberto como na 'luz="";'

//LDR PINO A0
String luz = "";

void setup() {
  Serial.begin(115200);
  //Inicia Sensor Temperatura e Umidade
  dht.begin();
 

  //Conectando a rede Wifi
  irsend.begin();
  Serial.println();                       
  Serial.println();                       
  
  WiFi.config(ip, gateway, subnet);
  
  Serial.print("Conectando a ");         
  Serial.println(ssid);                 
  WiFi.begin(ssid, password);
  
  
 //Verificando Conexão
 while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
 }
  Serial.println("");
  Serial.println("WiFi conectada");
  
  //Iniciando o servidor Web
  server.begin();
  Serial.println("Servidor Iniciado pelo IP:");
  Serial.println(WiFi.localIP());
}


void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  String request = client.readStringUntil('\r'); 
  Serial.println(request);
  toc = millis();                          
  client.flush();                                
  
  //Bloco InfraVermelho
  if (request.indexOf("/MENU") != -1)  {
      irsend.sendRaw(MENU,tamanho,frequencia); // PAR�METROS NECESS�RIOS PARA ENVIO DO SINAL IR
      Serial.println("Comando enviado: Aumenta canal");
      }

  if (request.indexOf("/MUTE") != -1)  {
      irsend.sendRaw(MUTE,tamanho,frequencia); // PAR�METROS NECESS�RIOS PARA ENVIO DO SINAL IR
      Serial.println("Comando enviado: Diminui canal");
      }

  if (request.indexOf("/VOLTAR") != -1)  {
      irsend.sendRaw(VOLTAR,tamanho,frequencia); // PAR�METROS NECESS�RIOS PARA ENVIO DO SINAL IR
      Serial.println("Comando enviado: Liga/Desliga TV");
      }
      
  if (request.indexOf("/VMAIS") != -1)  {
      irsend.sendRaw(VMAIS,tamanho,frequencia); // PAR�METROS NECESS�RIOS PARA ENVIO DO SINAL IR
      Serial.println("Comando enviado: Aumenta Volumo");
      }
      
   if (request.indexOf("/VMENOS") != -1)  {
      irsend.sendRaw(VMENOS,tamanho,frequencia); // PAR�METROS NECESS�RIOS PARA ENVIO DO SINAL IR
      Serial.println("Comando enviado: Diminui Volume");
      }

  //BLOCO TEMPERATURA
  int temperatura = dht.readTemperature();   //Define uma variavel para Temperatura
  Serial.print("Temperatura: "); 
  Serial.print(temperatura);
  Serial.println(" *C");

  //BLOCO SENSOR ILUMINAÇÃO

  int sensorValue = analogRead(0);           // Converte o dado de 0 a 1023 para 0-100.
  float voltage = sensorValue * (100 / 1023.0);
  int threshold=50; 
  Serial.println(voltage);
   if (voltage >= threshold){
    luz = "Desligada";
  }else {
    luz = "Ligada";
  } 

  //BLOCO SENSOR PRESENÇA
  switchValue = digitalRead(switchPin);
  Serial.println("Sensor de presença:");
  Serial.println(switchValue);
  if(switchValue==1)
  {
  senspren = "Ativo";
  }
  else  {
    senspren = "Inativo";
    } ;
  
  //Iniciando o buffer que ira conter a pagina HTML que sera enviada para o browser.
  String buf = "";
  buf += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  buf += "<head> ";
  buf += "<meta charset='UTF-8'> ";
  buf += "<meta http-equiv='cache-control' content='max-age=0' /> ";
  buf += "<meta http-equiv='cache-control' content='no-cache' /> ";
  buf += "<meta http-equiv='expires' content='0' /> ";
  buf += "<meta http-equiv='expires' content='Tue, 01 Jan 1980 1:00:00 GMT' /> ";
  buf += "<meta http-equiv='pragma' content='no-cache' /> ";
  buf += "<title>Automa&ccedil;&atilde;o Residencial</title> ";
  buf += "<style> ";
  buf += "body{font-family:Open Sans; color:#555555;} ";
  buf += "h1{font-size:24px; font-weight:normal; margin:0.4em 0;} ";
  buf += ".container { width: 100%; margin: 0 auto; } ";
  buf += ".container .row { float: left; clear: both; width: 100%; } ";
  buf += ".container .col { float: left; margin: 0 0 1.2em; padding-right: 1.2em; padding-left: 1.2em; } ";
  buf += ".container .col.four, .container .col.twelve { width: 100%; } ";
  buf += "@media screen and (min-width: 767px) { ";
  buf += ".container{width: 100%; max-width: 1080px; margin: 0 auto;} ";
  buf += ".container .row{width:100%; float:left; clear:both;} ";
  buf += ".container .col{float: left; margin: 0 0 1em; padding-right: .5em; padding-left: .5em;} ";
  buf += ".container .col.four { width: 50%; } ";
  buf += ".container .col.twelve { width: 100%; } ";
  buf += "} ";
  buf += "* {-moz-box-sizing: border-box; -webkit-box-sizing: border-box; box-sizing: border-box;} ";
  buf += "a{text-decoration:none;} ";
  buf += ".btn {font-size: 18px; white-space:nowrap; width:100%; padding:.8em 1.5em; font-family: Open Sans, Helvetica,Arial,sans-serif; ";
  buf += "line-height:18px; display: inline-block;zoom: 1; color: #fff; text-align: center; position:relative; ";
  buf += "-webkit-transition: border .25s linear, color .25s linear, background-color .25s linear; ";
  buf += "transition: border .25s linear, color .25s linear, background-color .25s linear;} ";
  buf += ".btn.btn-sea{background-color: #08bc9a; border-color: #08bc9a; -webkit-box-shadow: 0 3px 0 #088d74; box-shadow: 0 3px 0 #088d74;} ";
  buf += ".btn.btn-sea:hover{background-color:#01a183;} ";
  buf += ".btn.btn-sea:active{ top: 3px; outline: none; -webkit-box-shadow: none; box-shadow: none;} ";
  buf += "</style> ";
  buf += "</head> ";
  buf += "<body> ";
  //TV
  buf += "<u><b><p align='center'><font size='5'>Televis&atilde;o</font></p></b></u>";
  buf += "<a href=\"/VOLTAR\"\"><button>VOLTAR</button></a>";
  buf += "<a href=\"/MENU\"\"><button>MENU</button></a>";
  buf += "<a href=\"/MUTE\"\"><button>MUTE</button></a>";
  buf += "<a href=\"/VMAIS\"\"><button>VOL +</button></a>";
  buf += "<a href=\"/VMENOS\"\"><button>VOL -</button></a>";
  
  //SENSOR DE TEMPERATURA
  buf += "<div class='row'> ";
  buf += "<div class='col twelve'> ";
  buf += "<u><b><p align='center'><font size='5'>Sensor Temperatura</font></p></b></u>";
  buf += "</div> ";
  buf += "</div> ";
  buf += "<p align='left'><font size='4'>Temperatura: </font></p>";
  buf += temperatura;
  buf += "°C";
 
  //SENSOR DE LUMINOSIDADE
  buf += "<div class='row'> ";
  buf += "<div class='col twelve'> ";
  buf += "<u><b><p align='center'><font size='5'>Sensor Luminosidade</font></p></b></u>";
  buf += "</div> ";
  buf += "</div> ";  
  buf += "<p align='left'><font size='4'>Luz: </font></p> ";
  buf += luz;

  //SENSOR DE PRESENÇA
  buf += "<div class='row'> ";
  buf += "<div class='col twelve'> ";
  buf += "<u><b><p align='center'><font size='5'>Sensor de Presen&ccedil;a</font></p></b></u>";
  buf += "</div> ";
  buf += "</div> ";
  buf += "<p align='left'><font size='4'>Detec&ccedil;&atilde;o: </font></p>";
  buf += senspren;

  buf += "<i><b><p align='center'><font size='5'>Erick Ramos | Gustavo Campos | Danilo Santos</font></p></b></i>";
  buf += "</div> ";
  buf += "</body> ";
  buf += "</html> ";

  delay(1);
  //Enviando para o browser a 'pagina' criada.
  client.print(buf);
  tic = millis() - toc;
  tic = tic/12;
  client.print("Tempo de Processamento: ");
  client.print("Tempo de Processamento: ");
  client.print(tic);
  client.print(" ms");
  client.flush();

}
