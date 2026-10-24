#include "smartvagas.h"

void iniciarFrontEnd()
{
  server.serveStatic("/", LittleFS, "/")
      .setDefaultFile("index.html")
      .setCacheControl("no-store");
}
