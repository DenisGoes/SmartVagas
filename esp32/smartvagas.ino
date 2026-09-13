#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "sua-rede";
const char* password = "sua-senha";

const char* serverUrl = "https://smartvagas-api.onrender.com/smartvagas/leituras";


// GPIOs

// Sensor 1
const int trigger1 = 26;
const int echo1 = 27;

// Sensor 2
const int trigger2 = 32;
const int echo2 = 34;

// LEDs
const int led1 = 14;
const int led2 = 13;

// Estados das vagas
bool vaga1Ocupada = false;
bool vaga2Ocupada = false;

bool estadoAnterior1 = false;
bool estadoAnterior2 = false;



// FUNÇÃO PARA ENVIAR LEITURA
void enviarLeitura(int sensorId, bool estado) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi desconectado. Leitura não enviada.");
    return;
  }

  HTTPClient http;

  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String json = "{";

  json += "\"sensorId\":";
  json += sensorId;

  json += ",\"estado\":";
  json += estado ? "true" : "false";

  json += "}";

  Serial.println();
  Serial.println("Enviando leitura:");
  Serial.println(json);

  int resposta = http.POST(json);

  Serial.print("HTTP Status: ");
  Serial.println(resposta);

  String respostaApi = http.getString();

  Serial.println("Resposta da API:");
  Serial.println(respostaApi);

  http.end();
}


void setup() {
  Serial.begin(115200);

  // Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado");

  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // Sensor 1
  pinMode(trigger1, OUTPUT);
  pinMode(echo1, INPUT);

  // Sensor 2
  pinMode(trigger2, OUTPUT);
  pinMode(echo2, INPUT);

  // LEDs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}

void loop() {

  // SENSOR 1
  digitalWrite(trigger1, LOW);
  delayMicroseconds(2);

  digitalWrite(trigger1, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigger1, LOW);


  long duracao1 = pulseIn(echo1, HIGH);

  float distancia1 = (duracao1 * 0.0343) / 2;

  Serial.print("Sensor 1 - Distancia: ");
  Serial.print(distancia1);
  Serial.println(" cm");

  // Determina estado da vaga 1
  if (distancia1 < 10) {
    vaga1Ocupada = true;
  }
  else if (distancia1 > 10) {
    vaga1Ocupada = false;
  }

  // LED 1
  if (vaga1Ocupada) {
    digitalWrite(led1, HIGH);
  }
  else {
    digitalWrite(led1, LOW);
  }


  // VERIFICA SE VAGA 1 MUDOU
  if (vaga1Ocupada != estadoAnterior1) {
    Serial.println("ESTADO DA VAGA 1 MUDOU!");

    estadoAnterior1 = vaga1Ocupada;
    enviarLeitura(1, vaga1Ocupada);
  }

  // SENSOR 2
  digitalWrite(trigger2, LOW);
  delayMicroseconds(2);

  digitalWrite(trigger2, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigger2, LOW);


  long duracao2 = pulseIn(echo2, HIGH);

  float distancia2 = (duracao2 * 0.0343) / 2;


  Serial.print("Sensor 2 - Distancia: ");
  Serial.print(distancia2);
  Serial.println(" cm");


  // Determina estado da vaga 2
  if (distancia2 < 10) {
    vaga2Ocupada = true;
  }
  else if (distancia2 > 10) {
    vaga2Ocupada = false;
  }

  // LED 2
  if (vaga2Ocupada) {
    digitalWrite(led2, HIGH);
  }
  else {
    digitalWrite(led2, LOW);
  }

  // VERIFICA SE VAGA 2 MUDOU
  if (vaga2Ocupada != estadoAnterior2) {
    Serial.println("ESTADO DA VAGA 2 MUDOU!");

    estadoAnterior2 = vaga2Ocupada;
    enviarLeitura(2, vaga2Ocupada);
  }

  Serial.println("--------------------------");

  // Continua verificando os sensores
  // a cada 500 ms
  delay(500);
}

