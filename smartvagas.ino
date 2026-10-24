#include "smartvagas.h"

// Configuracao
const char *ssid = "NOME_DA_REDE_WIFI";
const char *password = "SENHA_DA_REDE_WIFI";
AsyncWebServer server(80);
JsonDocument db;

const int trigger1 = 26, echo1 = 27;
const int trigger2 = 32, echo2 = 34;
const int led1 = 14, led2 = 13;
const int SENSOR_ID_1 = 1, SENSOR_ID_2 = 2;
const float DISTANCIA_MIN = 2.0, DISTANCIA_MAX = 10.0;
const unsigned long TEMPO_CONFIRMACAO_ENTRADA = 5000;
const unsigned long TEMPO_CONFIRMACAO_SAIDA = 3000;
const unsigned long INTERVALO_PISCADA = 300, INTERVALO_LEITURA = 500;
unsigned long ultimaLeitura = 0;
ControleVaga controleVaga1 = {LIVRE, 0, 0, false};
ControleVaga controleVaga2 = {LIVRE, 0, 0, false};
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

void setup()
{
  Serial.begin(115200);
  if (!iniciarBanco()) return;

  pinMode(trigger1, OUTPUT); pinMode(echo1, INPUT);
  pinMode(trigger2, OUTPUT); pinMode(echo2, INPUT);
  pinMode(led1, OUTPUT); pinMode(led2, OUTPUT);
  digitalWrite(led1, controleVaga1.ledLigado ? HIGH : LOW);
  digitalWrite(led2, controleVaga2.ledLigado ? HIGH : LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("\nAcesse: http://"); Serial.println(WiFi.localIP());
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  iniciarFrontEnd();
  iniciarAPI();
}

void loop()
{
  unsigned long agora = millis();
  if (agora - ultimaLeitura >= INTERVALO_LEITURA)
  {
    ultimaLeitura = agora;
    processarSensores();
  }
}

