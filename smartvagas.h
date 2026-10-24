#pragma once

#include <ArduinoJson.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <time.h>

#define DB_PATH "/db.json"
#define MAX_LEITURAS 150

enum EstadoVaga { LIVRE, SUSPEITA, OCUPADA, CONFIRMANDO_SAIDA };

struct ControleVaga {
  EstadoVaga estado;
  unsigned long inicioEstado;
  unsigned long ultimaPiscada;
  bool ledLigado;
};

extern AsyncWebServer server;
extern JsonDocument db;
extern const int SENSOR_ID_1, SENSOR_ID_2;
extern ControleVaga controleVaga1, controleVaga2;

String horaAtualISO();
bool iniciarBanco();
void seedDB();
void saveDB();
void loadDB();
JsonObject buscarPorId(JsonArray arr, int id);
void sincronizarControleComBanco(int sensorId, ControleVaga &controle);
bool existeFilho(JsonArray arr, const char *campo, int id);
void enviarJson(AsyncWebServerRequest *request, int status, JsonVariant data);
void enviarTexto(AsyncWebServerRequest *request, int status, const String &msg);
float medirDistanciaCM(int trigPin, int echoPin);
bool distanciaValida(float distancia);
void registrarLeitura(int sensorId, bool estado);
void piscarLED(ControleVaga &controle, int pinoLED, unsigned long agora);
void processarVaga(float distancia, ControleVaga &controle, int pinoLED, int sensorId, unsigned long agora);
void processarSensores();
void iniciarAPI();
void iniciarFrontEnd();
void configurarRotasFixas();
void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
bool terminaComNumero(const String &url, const String &prefixo, int &id);
void tratarEstacionamentoComId(AsyncWebServerRequest *request, int id, const String &corpo);
void tratarVagaComId(AsyncWebServerRequest *request, int id, const String &corpo);
void tratarSensorComId(AsyncWebServerRequest *request, int id, const String &corpo);
void tratarLeituraComId(AsyncWebServerRequest *request, int id);
void tratarLeiturasPorSensor(AsyncWebServerRequest *request, int sensorId);
void handleNotFound(AsyncWebServerRequest *request);

