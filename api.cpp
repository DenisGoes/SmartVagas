#include "smartvagas.h"

void iniciarAPI()
{
  configurarRotasFixas();
  server.onRequestBody(handleBody);
  server.onNotFound(handleNotFound);
  server.begin();
}

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

  response->addHeader(
      "Cache-Control",
      "no-store, no-cache, must-revalidate");

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
                    arr.add<JsonObject>();

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
                    arr.add<JsonObject>();

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
                    arr.add<JsonObject>();

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

    JsonDocument doc;

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

    JsonDocument doc;

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

    JsonDocument doc;

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

  JsonDocument resultado;

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
