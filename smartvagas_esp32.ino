// =====================================================
// SMARTVAGAS - TUDO NO ESP32
// =====================================================
//
// Tudo roda local: ESP32 serve o front end (LittleFS) e a API REST
// dados num arquivo JSON no próprio LittleFS. Sem banco relacional.
//
// Bibliotecas necessárias:
//   - ESP32Async / ESPAsyncWebServer
//   - ESP32Async / AsyncTCP
//   - ArduinoJson (by Benoit Blanchon) - versão 6.x
//
// Placa: WROOM-32
// Sensores: 2x HC-SR04
// Front end: polling
// =====================================================

#include <WiFi.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <time.h>

// =====================================================
// WIFI
// =====================================================

const char *ssid = "smartvagas";
const char *password = "SENHA_DA_REDE_WIFI";

AsyncWebServer server(80);

// =====================================================
// "BANCO" EM JSON
// =====================================================

#define DB_PATH "/db.json"
#define DB_CAPACITY 32768
#define MAX_LEITURAS 150

DynamicJsonDocument db(DB_CAPACITY);

// =====================================================
// SENSORES ULTRASSÔNICOS E LEDS
// =====================================================

const int trigger1 = 26;
const int echo1 = 27;

const int trigger2 = 32;
const int echo2 = 34;

const int led1 = 14;
const int led2 = 13;

// =====================================================
// IDS DOS SENSORES
// =====================================================

const int SENSOR_ID_1 = 1;
const int SENSOR_ID_2 = 2;

// =====================================================
// CONFIGURAÇÃO DA DETECÇÃO
// =====================================================
//
// SOMENTE objetos dentro dessa faixa de distância
// serão considerados candidatos a ocupar a vaga.
//
// ATENÇÃO:
// Esses valores são INICIAIS para teste.
//
// Depois vamos medir fisicamente a maquete:
//
// Sensor -> chão
// Sensor -> carro
// Sensor -> pessoa
// Sensor -> cachorro/objeto baixo
//
// E então ajustaremos esses valores.
//
// Exemplo inicial:
// 10 cm até 16 cm
//

const float DISTANCIA_MIN = 10.0;
const float DISTANCIA_MAX = 16.0;

// =====================================================
// TEMPOS DA LÓGICA
// =====================================================

// Objeto precisa permanecer na faixa por 5 segundos
// para confirmar ocupação.
const unsigned long TEMPO_CONFIRMACAO_ENTRADA = 5000;

// Objeto precisa permanecer fora da faixa por 3 segundos
// para confirmar saída.
const unsigned long TEMPO_CONFIRMACAO_SAIDA = 3000;

// Intervalo do pisca-pisca.
const unsigned long INTERVALO_PISCADA = 300;

// Intervalo entre leituras dos sensores.
const unsigned long INTERVALO_LEITURA = 500;

// Controle do momento da última leitura.
unsigned long ultimaLeitura = 0;

// =====================================================
// ESTADOS DA VAGA
// =====================================================

enum EstadoVaga
{

  LIVRE,

  SUSPEITA,

  OCUPADA,

  CONFIRMANDO_SAIDA
};

// =====================================================
// CONTROLE INDIVIDUAL DA VAGA
// =====================================================

struct ControleVaga
{

  EstadoVaga estado;

  // Momento em que entrou no estado atual.
  unsigned long inicioEstado;

  // Controle do pisca-pisca.
  unsigned long ultimaPiscada;

  // Estado atual do LED.
  bool ledLigado;
};

// =====================================================
// CONTROLE DAS DUAS VAGAS
// =====================================================

ControleVaga controleVaga1 = {

    LIVRE,

    0,

    0,

    false};

ControleVaga controleVaga2 = {

    LIVRE,

    0,

    0,

    false};

// =====================================================
// DATA / HORA
// =====================================================

String horaAtualISO()
{

  time_t agora;

  time(&agora);

  struct tm timeinfo;

  localtime_r(
      &agora,
      &timeinfo);

  char buf[25];

  strftime(
      buf,
      sizeof(buf),
      "%Y-%m-%dT%H:%M:%S",
      &timeinfo);

  return String(buf);
}

// =====================================================
// BANCO - SEED
// =====================================================

void seedDB()
{

  db.clear();

  // ---------------------------------------------------
  // PRÓXIMOS IDS
  // ---------------------------------------------------

  JsonObject nextIds =
      db.createNestedObject("nextIds");

  nextIds["estacionamento"] = 2;

  nextIds["vaga"] = 3;

  nextIds["sensor"] = 3;

  nextIds["leitura"] = 1;

  // ---------------------------------------------------
  // ESTACIONAMENTO
  // ---------------------------------------------------

  JsonArray estacionamentos =
      db.createNestedArray(
          "estacionamentos");

  JsonObject e1 =
      estacionamentos.createNestedObject();

  e1["id"] = 1;

  e1["nome"] =
      "Estacionamento Principal";

  // ---------------------------------------------------
  // VAGAS
  // ---------------------------------------------------

  JsonArray vagas =
      db.createNestedArray(
          "vagas");

  JsonObject v1 =
      vagas.createNestedObject();

  v1["id"] = 1;
  v1["numero"] = "A01";
  v1["estacionamentoId"] = 1;
  v1["estado"] = false;

  JsonObject v2 =
      vagas.createNestedObject();

  v2["id"] = 2;
  v2["numero"] = "A02";
  v2["estacionamentoId"] = 1;
  v2["estado"] = false;

  // ---------------------------------------------------
  // SENSORES
  // ---------------------------------------------------

  JsonArray sensores =
      db.createNestedArray(
          "sensores");

  JsonObject s1 =
      sensores.createNestedObject();

  s1["id"] = 1;
  s1["nome"] = "Sensor Vaga A01";
  s1["vagaId"] = 1;

  JsonObject s2 =
      sensores.createNestedObject();

  s2["id"] = 2;
  s2["nome"] = "Sensor Vaga A02";
  s2["vagaId"] = 2;

  // ---------------------------------------------------
  // LEITURAS
  // ---------------------------------------------------

  db.createNestedArray(
      "leituras");
}

// =====================================================
// SALVAR BANCO
// =====================================================

void saveDB()
{

  File file =
      LittleFS.open(
          DB_PATH,
          "w");

  if (!file)
  {

    Serial.println(
        "Erro ao abrir db.json para escrita");

    return;
  }

  serializeJson(
      db,
      file);

  file.close();
}

// =====================================================
// CARREGAR BANCO
// =====================================================

void loadDB()
{

  if (!LittleFS.exists(DB_PATH))
  {

    Serial.println(
        "db.json não existe, criando dados iniciais...");

    seedDB();

    saveDB();

    return;
  }

  File file =
      LittleFS.open(
          DB_PATH,
          "r");

  DeserializationError err =
      deserializeJson(
          db,
          file);

  file.close();

  if (err)
  {

    Serial.println(
        "db.json corrompido, recriando...");

    seedDB();

    saveDB();
  }
}

// =====================================================
// BUSCAR POR ID
// =====================================================

JsonObject buscarPorId(
    JsonArray arr,
    int id)
{

  for (
      JsonObject item : arr)
  {

    if (
        item["id"] == id)
    {

      return item;
    }
  }

  return JsonObject();
}

// =====================================================
// VERIFICAR FILHOS
// =====================================================

bool existeFilho(
    JsonArray arr,
    const char *campo,
    int id)
{

  for (
      JsonObject item : arr)
  {

    if (
        item[campo] == id)
    {

      return true;
    }
  }

  return false;
}

// =====================================================
// ENVIAR JSON
// =====================================================

void enviarJson(
    AsyncWebServerRequest *request,
    int status,
    JsonVariant data)
{

  AsyncResponseStream *response =
      request->beginResponseStream(
          "application/json");

  response->setCode(
      status);

  serializeJson(
      data,
      *response);

  request->send(
      response);
}

// =====================================================
// ENVIAR TEXTO
// =====================================================

void enviarTexto(
    AsyncWebServerRequest *request,
    int status,
    const String &msg)
{

  request->send(
      status,
      "text/plain; charset=utf-8",
      msg);
}

// =====================================================
// MEDIR DISTÂNCIA
// =====================================================

float medirDistanciaCM(
    int trigPin,
    int echoPin)
{

  digitalWrite(
      trigPin,
      LOW);

  delayMicroseconds(
      2);

  digitalWrite(
      trigPin,
      HIGH);

  delayMicroseconds(
      10);

  digitalWrite(
      trigPin,
      LOW);

  long duracao =
      pulseIn(
          echoPin,
          HIGH,
          30000);

  if (
      duracao == 0)
  {

    return -1;
  }

  float distancia =
      (duracao * 0.0343) / 2;

  return distancia;
}

// =====================================================
// VERIFICAR FAIXA DE DISTÂNCIA
// =====================================================

bool distanciaValida(
    float distancia)
{

  if (
      distancia < 0)
  {

    return false;
  }

  return (
      distancia >= DISTANCIA_MIN && distancia <= DISTANCIA_MAX);
}

// =====================================================
// REGISTRAR LEITURA
// =====================================================

void registrarLeitura(
    int sensorId,
    bool estado)
{

  JsonArray sensores =
      db["sensores"]
          .as<JsonArray>();

  JsonObject sensor =
      buscarPorId(
          sensores,
          sensorId);

  if (
      sensor.isNull())
  {

    Serial.println(
        "Sensor não encontrado para registrar leitura.");

    return;
  }

  int vagaId =
      sensor["vagaId"];

  JsonArray vagas =
      db["vagas"]
          .as<JsonArray>();

  JsonObject vaga =
      buscarPorId(
          vagas,
          vagaId);

  if (
      !vaga.isNull())
  {

    vaga["estado"] =
        estado;
  }

  JsonArray leituras =
      db["leituras"]
          .as<JsonArray>();

  JsonObject nova =
      leituras.createNestedObject();

  int id =
      db["nextIds"]["leitura"];

  nova["id"] =
      id;

  nova["sensorId"] =
      sensorId;

  nova["estado"] =
      estado;

  nova["dataHora"] =
      horaAtualISO();

  db["nextIds"]["leitura"] =
      id + 1;

  // ---------------------------------------------------
  // LIMITAR HISTÓRICO
  // ---------------------------------------------------

  while (
      leituras.size() > MAX_LEITURAS)
  {

    leituras.remove(
        0);
  }

  saveDB();

  // ---------------------------------------------------
  // SERIAL
  // ---------------------------------------------------

  Serial.print(
      "Leitura registrada -> sensor ");

  Serial.print(
      sensorId);

  Serial.print(
      " | vaga ");

  Serial.print(
      vagaId);

  Serial.print(
      " | estado: ");

  Serial.println(
      estado
          ? "OCUPADA"
          : "LIVRE");
}

// =====================================================
// PISCAR LED
// =====================================================

void piscarLED(
    ControleVaga &controle,
    int pinoLED,
    unsigned long agora)
{

  if (
      agora - controle.ultimaPiscada >= INTERVALO_PISCADA)
  {

    controle.ultimaPiscada =
        agora;

    controle.ledLigado =
        !controle.ledLigado;

    digitalWrite(
        pinoLED,
        controle.ledLigado
            ? HIGH
            : LOW);
  }
}

// =====================================================
// PROCESSAR UMA VAGA
// =====================================================

void processarVaga(
    float distancia,
    ControleVaga &controle,
    int pinoLED,
    int sensorId,
    unsigned long agora)
{

  bool dentroDaFaixa =
      distanciaValida(
          distancia);

  // ===================================================
  // LIVRE
  // ===================================================

  if (
      controle.estado == LIVRE)
  {

    digitalWrite(
        pinoLED,
        LOW);

    controle.ledLigado =
        false;

    // Objeto entrou na faixa.
    if (
        dentroDaFaixa)
    {

      controle.estado =
          SUSPEITA;

      controle.inicioEstado =
          agora;

      controle.ultimaPiscada =
          agora;

      controle.ledLigado =
          true;

      digitalWrite(
          pinoLED,
          HIGH);

      Serial.print(
          "Sensor ");

      Serial.print(
          sensorId);

      Serial.println(
          ": objeto detectado -> SUSPEITA");
    }

    return;
  }

  // ===================================================
  // SUSPEITA
  // ===================================================

  if (
      controle.estado == SUSPEITA)
  {

    // LED piscando.
    piscarLED(
        controle,
        pinoLED,
        agora);

    // -------------------------------------------------
    // Saiu antes dos 5 segundos.
    // -------------------------------------------------

    if (
        !dentroDaFaixa)
    {

      controle.estado =
          LIVRE;

      controle.ledLigado =
          false;

      digitalWrite(
          pinoLED,
          LOW);

      Serial.print(
          "Sensor ");

      Serial.print(
          sensorId);

      Serial.println(
          ": objeto saiu antes de 5s -> LIVRE");

      return;
    }

    // -------------------------------------------------
    // Permaneceu 5 segundos.
    // -------------------------------------------------

    if (
        agora - controle.inicioEstado >= TEMPO_CONFIRMACAO_ENTRADA)
    {

      controle.estado =
          OCUPADA;

      controle.ledLigado =
          true;

      digitalWrite(
          pinoLED,
          HIGH);

      Serial.print(
          "Sensor ");

      Serial.print(
          sensorId);

      Serial.println(
          ": ocupação confirmada -> OCUPADA");

      registrarLeitura(
          sensorId,
          true);
    }

    return;
  }

  // ===================================================
  // OCUPADA
  // ===================================================

  if (
      controle.estado == OCUPADA)
  {

    digitalWrite(
        pinoLED,
        HIGH);

    controle.ledLigado =
        true;

    // -------------------------------------------------
    // Objeto saiu da faixa.
    // -------------------------------------------------

    if (
        !dentroDaFaixa)
    {

      controle.estado =
          CONFIRMANDO_SAIDA;

      controle.inicioEstado =
          agora;

      controle.ultimaPiscada =
          agora;

      controle.ledLigado =
          true;

      Serial.print(
          "Sensor ");

      Serial.print(
          sensorId);

      Serial.println(
          ": objeto saiu -> confirmando saída");
    }

    return;
  }

  // ===================================================
  // CONFIRMANDO SAÍDA
  // ===================================================

  if (
      controle.estado == CONFIRMANDO_SAIDA)
  {

    // LED piscando.
    piscarLED(
        controle,
        pinoLED,
        agora);

    // -------------------------------------------------
    // Objeto voltou.
    // -------------------------------------------------

    if (
        dentroDaFaixa)
    {

      controle.estado =
          OCUPADA;

      controle.ledLigado =
          true;

      digitalWrite(
          pinoLED,
          HIGH);

      Serial.print(
          "Sensor ");

      Serial.print(
          sensorId);

      Serial.println(
          ": objeto voltou -> OCUPADA");

      return;
    }

    // -------------------------------------------------
    // Ficou fora por 3 segundos.
    // -------------------------------------------------

    if (
        agora - controle.inicioEstado >= TEMPO_CONFIRMACAO_SAIDA)
    {

      controle.estado =
          LIVRE;

      controle.ledLigado =
          false;

      digitalWrite(
          pinoLED,
          LOW);

      Serial.print(
          "Sensor ");

      Serial.print(
          sensorId);

      Serial.println(
          ": saída confirmada -> LIVRE");

      registrarLeitura(
          sensorId,
          false);
    }

    return;
  }
}

// =====================================================
// PROCESSAR OS SENSORES
// =====================================================

void processarSensores()
{

  unsigned long agora =
      millis();

  // ===================================================
  // SENSOR 1
  // ===================================================

  float distancia1 =
      medirDistanciaCM(
          trigger1,
          echo1);

  Serial.print(
      "Sensor 1: ");

  if (
      distancia1 < 0)
  {

    Serial.println(
        "sem eco");
  }
  else
  {

    Serial.print(
        distancia1);

    Serial.println(
        " cm");
  }

  processarVaga(
      distancia1,
      controleVaga1,
      led1,
      SENSOR_ID_1,
      agora);

  // ===================================================
  // SENSOR 2
  // ===================================================

  float distancia2 =
      medirDistanciaCM(
          trigger2,
          echo2);

  Serial.print(
      "Sensor 2: ");

  if (
      distancia2 < 0)
  {

    Serial.println(
        "sem eco");
  }
  else
  {

    Serial.print(
        distancia2);

    Serial.println(
        " cm");
  }

  processarVaga(
      distancia2,
      controleVaga2,
      led2,
      SENSOR_ID_2,
      agora);
}

// =====================================================
// ROTAS FIXAS
// =====================================================

void configurarRotasFixas()
{

  // ===================================================
  // ESTACIONAMENTOS
  // ===================================================

  server.on(
      "/smartvagas/estacionamentos",
      HTTP_GET,
      [](AsyncWebServerRequest *request)
      {
        enviarJson(
            request,
            200,
            db["estacionamentos"]);
      });

  AsyncCallbackJsonWebHandler *
      criarEstacionamento =
          new AsyncCallbackJsonWebHandler(
              "/smartvagas/estacionamentos",
              [](AsyncWebServerRequest *request,
                 JsonVariant &json)
              {
                JsonObject body =
                    json.as<JsonObject>();

                JsonArray arr =
                    db["estacionamentos"]
                        .as<JsonArray>();

                int id =
                    db["nextIds"]["estacionamento"];

                db["nextIds"]["estacionamento"] =
                    id + 1;

                JsonObject novo =
                    arr.createNestedObject();

                novo["id"] =
                    id;

                novo["nome"] =
                    body["nome"];

                saveDB();

                enviarTexto(
                    request,
                    200,
                    "Estacionamento " + String((const char *)body["nome"]) + " criado com sucesso!");
              });

  criarEstacionamento->setMethod(
      HTTP_POST);

  server.addHandler(
      criarEstacionamento);

  // ===================================================
  // VAGAS
  // ===================================================

  server.on(
      "/smartvagas/vagas",
      HTTP_GET,
      [](AsyncWebServerRequest *request)
      {
        enviarJson(
            request,
            200,
            db["vagas"]);
      });

  AsyncCallbackJsonWebHandler *
      criarVaga =
          new AsyncCallbackJsonWebHandler(
              "/smartvagas/vagas",
              [](AsyncWebServerRequest *request,
                 JsonVariant &json)
              {
                JsonObject body =
                    json.as<JsonObject>();

                JsonArray arr =
                    db["vagas"]
                        .as<JsonArray>();

                int id =
                    db["nextIds"]["vaga"];

                db["nextIds"]["vaga"] =
                    id + 1;

                JsonObject novo =
                    arr.createNestedObject();

                novo["id"] =
                    id;

                novo["numero"] =
                    body["numero"];

                novo["estacionamentoId"] =
                    body["estacionamentoId"];

                novo["estado"] =
                    body["estado"] | false;

                saveDB();

                enviarTexto(
                    request,
                    200,
                    "Vaga " + String((const char *)body["numero"]) + " criada com sucesso!");
              });

  criarVaga->setMethod(
      HTTP_POST);

  server.addHandler(
      criarVaga);

  // ===================================================
  // SENSORES
  // ===================================================

  server.on(
      "/smartvagas/sensores",
      HTTP_GET,
      [](AsyncWebServerRequest *request)
      {
        enviarJson(
            request,
            200,
            db["sensores"]);
      });

  AsyncCallbackJsonWebHandler *
      criarSensor =
          new AsyncCallbackJsonWebHandler(
              "/smartvagas/sensores",
              [](AsyncWebServerRequest *request,
                 JsonVariant &json)
              {
                JsonObject body =
                    json.as<JsonObject>();

                JsonArray arr =
                    db["sensores"]
                        .as<JsonArray>();

                int id =
                    db["nextIds"]["sensor"];

                db["nextIds"]["sensor"] =
                    id + 1;

                JsonObject novo =
                    arr.createNestedObject();

                novo["id"] =
                    id;

                novo["nome"] =
                    body["nome"];

                novo["vagaId"] =
                    body["vagaId"];

                saveDB();

                enviarTexto(
                    request,
                    200,
                    "Sensor " + String((const char *)body["nome"]) + " cadastrado com sucesso!");
              });

  criarSensor->setMethod(
      HTTP_POST);

  server.addHandler(
      criarSensor);

  // ===================================================
  // LEITURAS
  // ===================================================

  server.on(
      "/smartvagas/leituras",
      HTTP_GET,
      [](AsyncWebServerRequest *request)
      {
        enviarJson(
            request,
            200,
            db["leituras"]);
      });

  server.on(
      "/smartvagas/leituras",
      HTTP_DELETE,
      [](AsyncWebServerRequest *request)
      {
        db["leituras"].to<JsonArray>();

        saveDB();

        enviarTexto(
            request,
            200,
            "Todas as leituras foram deletadas com sucesso!");
      });
}

// =====================================================
// ACUMULAR BODY
// =====================================================

void handleBody(
    AsyncWebServerRequest *request,
    uint8_t *data,
    size_t len,
    size_t index,
    size_t total)
{

  if (
      index == 0)
  {

    request->_tempObject =
        (void *)new String();
  }

  String *buf =
      (String *)request->_tempObject;

  if (buf)
  {

    buf->concat(
        (const char *)data,
        len);
  }
}

// =====================================================
// VERIFICAR URL COM ID
// =====================================================

bool terminaComNumero(
    const String &url,
    const String &prefixo,
    int &id)
{

  if (
      !url.startsWith(
          prefixo))
  {

    return false;
  }

  String resto =
      url.substring(
          prefixo.length());

  if (
      resto.length() == 0)
  {

    return false;
  }

  for (
      unsigned int i = 0;
      i < resto.length();
      i++)
  {

    if (
        !isDigit(
            resto[i]))
    {

      return false;
    }
  }

  id =
      resto.toInt();

  return true;
}

// =====================================================
// ESTACIONAMENTO POR ID
// =====================================================

void tratarEstacionamentoComId(
    AsyncWebServerRequest *request,
    int id,
    const String &corpo)
{

  JsonArray arr =
      db["estacionamentos"]
          .as<JsonArray>();

  JsonObject item =
      buscarPorId(
          arr,
          id);

  auto metodo =
      request->method();

  // ---------------------------------------------------
  // GET
  // ---------------------------------------------------

  if (
      metodo == HTTP_GET)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Estacionamento não encontrado.");

      return;
    }

    enviarJson(
        request,
        200,
        item);

    return;
  }

  // ---------------------------------------------------
  // PUT
  // ---------------------------------------------------

  if (
      metodo == HTTP_PUT)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Estacionamento não encontrado.");

      return;
    }

    DynamicJsonDocument doc(
        512);

    if (
        deserializeJson(
            doc,
            corpo))
    {

      enviarTexto(
          request,
          400,
          "JSON inválido.");

      return;
    }

    item["nome"] =
        doc.as<JsonObject>()["nome"];

    saveDB();

    enviarTexto(
        request,
        200,
        "Estacionamento atualizado com sucesso!");

    return;
  }

  // ---------------------------------------------------
  // DELETE
  // ---------------------------------------------------

  if (
      metodo == HTTP_DELETE)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Estacionamento não encontrado para deletar.");

      return;
    }

    if (
        existeFilho(
            db["vagas"].as<JsonArray>(),
            "estacionamentoId",
            id))
    {

      enviarTexto(
          request,
          200,
          "Erro: Estacionamento não encontrado.");

      return;
    }

    for (
        size_t i = 0;
        i < arr.size();
        i++)
    {

      if (
          arr[i]["id"] == id)
      {

        arr.remove(i);

        break;
      }
    }

    saveDB();

    enviarTexto(
        request,
        200,
        "Estacionamento deletado com sucesso!");

    return;
  }

  enviarTexto(
      request,
      405,
      "Método não suportado.");
}

// =====================================================
// VAGA POR ID
// =====================================================

void tratarVagaComId(
    AsyncWebServerRequest *request,
    int id,
    const String &corpo)
{

  JsonArray arr =
      db["vagas"]
          .as<JsonArray>();

  JsonObject item =
      buscarPorId(
          arr,
          id);

  auto metodo =
      request->method();

  // GET
  if (
      metodo == HTTP_GET)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Vaga não encontrada.");

      return;
    }

    enviarJson(
        request,
        200,
        item);

    return;
  }

  // PUT
  if (
      metodo == HTTP_PUT)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Vaga não encontrada.");

      return;
    }

    DynamicJsonDocument doc(
        512);

    if (
        deserializeJson(
            doc,
            corpo))
    {

      enviarTexto(
          request,
          400,
          "JSON inválido.");

      return;
    }

    JsonObject body =
        doc.as<JsonObject>();

    item["numero"] =
        body["numero"];

    item["estacionamentoId"] =
        body["estacionamentoId"];

    item["estado"] =
        body["estado"] | false;

    saveDB();

    enviarTexto(
        request,
        200,
        "Vaga atualizada com sucesso!");

    return;
  }

  // DELETE
  if (
      metodo == HTTP_DELETE)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Vaga não encontrada para deletar.");

      return;
    }

    if (
        existeFilho(
            db["sensores"].as<JsonArray>(),
            "vagaId",
            id))
    {

      enviarTexto(
          request,
          200,
          "Erro: Vaga não encontrada.");

      return;
    }

    for (
        size_t i = 0;
        i < arr.size();
        i++)
    {

      if (
          arr[i]["id"] == id)
      {

        arr.remove(i);

        break;
      }
    }

    saveDB();

    enviarTexto(
        request,
        200,
        "Vaga deletada com sucesso!");

    return;
  }

  enviarTexto(
      request,
      405,
      "Método não suportado.");
}

// =====================================================
// SENSOR POR ID
// =====================================================

void tratarSensorComId(
    AsyncWebServerRequest *request,
    int id,
    const String &corpo)
{

  JsonArray arr =
      db["sensores"]
          .as<JsonArray>();

  JsonObject item =
      buscarPorId(
          arr,
          id);

  auto metodo =
      request->method();

  // GET
  if (
      metodo == HTTP_GET)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Sensor não encontrado.");

      return;
    }

    enviarJson(
        request,
        200,
        item);

    return;
  }

  // PUT
  if (
      metodo == HTTP_PUT)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Sensor não encontrado.");

      return;
    }

    DynamicJsonDocument doc(
        512);

    if (
        deserializeJson(
            doc,
            corpo))
    {

      enviarTexto(
          request,
          400,
          "JSON inválido.");

      return;
    }

    JsonObject body =
        doc.as<JsonObject>();

    item["nome"] =
        body["nome"];

    item["vagaId"] =
        body["vagaId"];

    saveDB();

    enviarTexto(
        request,
        200,
        "Sensor atualizado com sucesso!");

    return;
  }

  // DELETE
  if (
      metodo == HTTP_DELETE)
  {

    if (
        item.isNull())
    {

      enviarTexto(
          request,
          404,
          "Erro: Sensor não encontrado para deletar.");

      return;
    }

    if (
        existeFilho(
            db["leituras"].as<JsonArray>(),
            "sensorId",
            id))
    {

      enviarTexto(
          request,
          200,
          "Erro: Sensor não encontrado.");

      return;
    }

    for (
        size_t i = 0;
        i < arr.size();
        i++)
    {

      if (
          arr[i]["id"] == id)
      {

        arr.remove(i);

        break;
      }
    }

    saveDB();

    enviarTexto(
        request,
        200,
        "Sensor deletado com sucesso!");

    return;
  }

  enviarTexto(
      request,
      405,
      "Método não suportado.");
}

// =====================================================
// LEITURA POR ID
// =====================================================

void tratarLeituraComId(
    AsyncWebServerRequest *request,
    int id)
{

  JsonArray arr =
      db["leituras"]
          .as<JsonArray>();

  auto metodo =
      request->method();

  // GET
  if (
      metodo == HTTP_GET)
  {

    JsonObject item =
        buscarPorId(
            arr,
            id);

    if (
        item.isNull())
    {

      request->send(
          200,
          "application/json",
          "null");

      return;
    }

    enviarJson(
        request,
        200,
        item);

    return;
  }

  // DELETE
  if (
      metodo == HTTP_DELETE)
  {

    bool achou =
        false;

    for (
        size_t i = 0;
        i < arr.size();
        i++)
    {

      if (
          arr[i]["id"] == id)
      {

        arr.remove(i);

        achou =
            true;

        break;
      }
    }

    if (!achou)
    {

      enviarTexto(
          request,
          404,
          "Leitura não encontrada para deletar.");

      return;
    }

    saveDB();

    enviarTexto(
        request,
        200,
        "Leitura deletada com sucesso!");

    return;
  }

  enviarTexto(
      request,
      405,
      "Método não suportado.");
}

// =====================================================
// LEITURAS POR SENSOR
// =====================================================

void tratarLeiturasPorSensor(
    AsyncWebServerRequest *request,
    int sensorId)
{

  DynamicJsonDocument resultado(
      DB_CAPACITY / 2);

  JsonArray arr =
      resultado.to<JsonArray>();

  for (
      JsonObject item :
      db["leituras"].as<JsonArray>())
  {

    if (
        item["sensorId"] == sensorId)
    {

      arr.add(item);
    }
  }

  enviarJson(
      request,
      200,
      arr);
}

// =====================================================
// NOT FOUND
// =====================================================

void handleNotFound(
    AsyncWebServerRequest *request)
{

  String url =
      request->url();

  String corpo;

  if (
      request->_tempObject)
  {

    String *buf =
        (String *)request->_tempObject;

    corpo =
        *buf;

    delete buf;

    request->_tempObject =
        nullptr;
  }

  int id;

  // ---------------------------------------------------
  // LEITURAS POR SENSOR
  // ---------------------------------------------------

  if (
      request->method() == HTTP_GET && terminaComNumero(url, "/smartvagas/leituras/sensor/", id))
  {

    tratarLeiturasPorSensor(
        request,
        id);

    return;
  }

  // ---------------------------------------------------
  // ESTACIONAMENTOS
  // ---------------------------------------------------

  if (
      terminaComNumero(
          url,
          "/smartvagas/estacionamentos/",
          id))
  {

    tratarEstacionamentoComId(
        request,
        id,
        corpo);

    return;
  }

  // ---------------------------------------------------
  // VAGAS
  // ---------------------------------------------------

  if (
      terminaComNumero(
          url,
          "/smartvagas/vagas/",
          id))
  {

    tratarVagaComId(
        request,
        id,
        corpo);

    return;
  }

  // ---------------------------------------------------
  // SENSORES
  // ---------------------------------------------------

  if (
      terminaComNumero(
          url,
          "/smartvagas/sensores/",
          id))
  {

    tratarSensorComId(
        request,
        id,
        corpo);

    return;
  }

  // ---------------------------------------------------
  // LEITURAS
  // ---------------------------------------------------

  if (
      terminaComNumero(
          url,
          "/smartvagas/leituras/",
          id))
  {

    tratarLeituraComId(
        request,
        id);

    return;
  }

  request->send(
      404,
      "text/plain",
      "Rota não encontrada.");
}

// =====================================================
// SETUP
// =====================================================

void setup()
{

  Serial.begin(
      115200);

  // ===================================================
  // LITTLEFS
  // ===================================================

  if (
      !LittleFS.begin(true))
  {

    Serial.println(
        "Falha ao montar LittleFS!");

    return;
  }

  loadDB();

  // ===================================================
  // PINOS DOS SENSORES
  // ===================================================

  pinMode(
      trigger1,
      OUTPUT);

  pinMode(
      echo1,
      INPUT);

  pinMode(
      trigger2,
      OUTPUT);

  pinMode(
      echo2,
      INPUT);

  // ===================================================
  // PINOS DOS LEDS
  // ===================================================

  pinMode(
      led1,
      OUTPUT);

  pinMode(
      led2,
      OUTPUT);

  digitalWrite(
      led1,
      LOW);

  digitalWrite(
      led2,
      LOW);

  // ===================================================
  // WIFI
  // ===================================================

  WiFi.begin(
      ssid,
      password);

  Serial.print(
      "Conectando");

  while (
      WiFi.status() != WL_CONNECTED)
  {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.print(
      "Wi-Fi conectado! Acesse: http://");

  Serial.println(
      WiFi.localIP());

  // ===================================================
  // NTP
  // ===================================================

  configTime(
      -3 * 3600,
      0,
      "pool.ntp.org",
      "time.nist.gov");

  // ===================================================
  // FRONT END
  // ===================================================

  server
      .serveStatic(
          "/",
          LittleFS,
          "/")
      .setDefaultFile(
          "index.html");

  // ===================================================
  // ROTAS
  // ===================================================

  configurarRotasFixas();

  server.onRequestBody(
      handleBody);

  server.onNotFound(
      handleNotFound);

  // ===================================================
  // SERVIDOR
  // ===================================================

  server.begin();

  Serial.println(
      "Servidor iniciado.");

  Serial.println(
      "======================================");

  Serial.println(
      "CONFIGURACAO DA DETECCAO");

  Serial.println(
      "======================================");

  Serial.print(
      "Distancia minima: ");

  Serial.print(
      DISTANCIA_MIN);

  Serial.println(
      " cm");

  Serial.print(
      "Distancia maxima: ");

  Serial.print(
      DISTANCIA_MAX);

  Serial.println(
      " cm");

  Serial.print(
      "Confirmacao entrada: ");

  Serial.print(
      TEMPO_CONFIRMACAO_ENTRADA / 1000);

  Serial.println(
      " segundos");

  Serial.print(
      "Confirmacao saida: ");

  Serial.print(
      TEMPO_CONFIRMACAO_SAIDA / 1000);

  Serial.println(
      " segundos");

  Serial.println(
      "======================================");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{

  unsigned long agora =
      millis();

  if (
      agora - ultimaLeitura >= INTERVALO_LEITURA)
  {

    ultimaLeitura =
        agora;

    processarSensores();
  }
}