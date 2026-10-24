#include "smartvagas.h"

bool iniciarBanco()
{
  if (!LittleFS.begin(true))
  {
    Serial.println("Falha ao montar LittleFS!");
    return false;
  }

  loadDB();
  sincronizarControleComBanco(SENSOR_ID_1, controleVaga1);
  sincronizarControleComBanco(SENSOR_ID_2, controleVaga2);
  return true;
}

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
      db["nextIds"].to<JsonObject>();

  nextIds["estacionamento"] = 2;

  nextIds["vaga"] = 3;

  nextIds["sensor"] = 3;

  nextIds["leitura"] = 1;

  // ---------------------------------------------------
  // ESTACIONAMENTO
  // ---------------------------------------------------

  JsonArray estacionamentos =
      db["estacionamentos"].to<JsonArray>();

  JsonObject e1 =
      estacionamentos.add<JsonObject>();

  e1["id"] = 1;

  e1["nome"] =
      "Estacionamento Principal";

  // ---------------------------------------------------
  // VAGAS
  // ---------------------------------------------------

  JsonArray vagas =
      db["vagas"].to<JsonArray>();

  JsonObject v1 =
      vagas.add<JsonObject>();

  v1["id"] = 1;
  v1["numero"] = "A01";
  v1["estacionamentoId"] = 1;
  v1["estado"] = false;

  JsonObject v2 =
      vagas.add<JsonObject>();

  v2["id"] = 2;
  v2["numero"] = "A02";
  v2["estacionamentoId"] = 1;
  v2["estado"] = false;

  // ---------------------------------------------------
  // SENSORES
  // ---------------------------------------------------

  JsonArray sensores =
      db["sensores"].to<JsonArray>();

  JsonObject s1 =
      sensores.add<JsonObject>();

  s1["id"] = 1;
  s1["nome"] = "Sensor Vaga A01";
  s1["vagaId"] = 1;

  JsonObject s2 =
      sensores.add<JsonObject>();

  s2["id"] = 2;
  s2["nome"] = "Sensor Vaga A02";
  s2["vagaId"] = 2;

  // ---------------------------------------------------
  // LEITURAS
  // ---------------------------------------------------

  db["leituras"].to<JsonArray>();
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

// Sincroniza o controle em RAM com o último estado salvo. Assim,
// uma vaga ocupada no banco pode confirmar a liberação após reiniciar.

void sincronizarControleComBanco(
    int sensorId,
    ControleVaga &controle)
{

  JsonObject sensor =
      buscarPorId(
          db["sensores"].as<JsonArray>(),
          sensorId);

  if (sensor.isNull())
  {
    controle.estado = LIVRE;
    return;
  }

  int vagaId = sensor["vagaId"] | 0;
  JsonObject vaga =
      buscarPorId(
          db["vagas"].as<JsonArray>(),
          vagaId);

  controle.estado =
      !vaga.isNull() && (bool)(vaga["estado"] | false)
          ? OCUPADA
          : LIVRE;

  controle.inicioEstado = millis();
  controle.ledLigado = controle.estado == OCUPADA;
}

// =====================================================
// VERIFICAR FILHOS
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
      leituras.add<JsonObject>();

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
