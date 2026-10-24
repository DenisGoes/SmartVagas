# SmartVagas

Sistema local de estacionamento com ESP32. Dois sensores ultrassônicos atualizam as vagas e o site é servido pelo próprio ESP32, sem hospedagem externa.

## Hardware

| Item | GPIO |
| --- | --- |
| Sensor 1: TRIG / ECHO | 26 / 27 |
| Sensor 2: TRIG / ECHO | 32 / 34 |
| LED da vaga 1 / vaga 2 | 14 / 13 |

O código considera ocupada a faixa de 2 a 10 cm, após 5 segundos. A liberação é confirmada após 3 segundos fora dessa faixa. Ajuste esses valores no início de `smartvagas.ino`, se necessário.

> O pino ECHO de muitos HC-SR04 fornece 5 V. Use divisor de tensão antes de ligar ao ESP32, que trabalha com lógica de 3,3 V.

## Requisitos

- Arduino IDE 2.x e pacote de placas **esp32 by Espressif Systems**.
- Bibliotecas: **ArduinoJson**, **AsyncTCP** e **ESPAsyncWebServer**.
- Plugin **LittleFS Filesystem Uploader** para enviar os arquivos de `data/`.

As versões usadas neste projeto são Arduino IDE 2.3.10, ESP32 3.3.12, ArduinoJson 7.4.3, AsyncTCP 3.5.0, ESPAsyncWebServer 3.12.1 e LittleFS Uploader 1.6.3.

## Como executar

1. Abra `smartvagas.ino` no Arduino IDE.
2. No início do arquivo, informe o nome e a senha da rede Wi-Fi em `ssid` e `password`.
3. Instale as bibliotecas acima pelo Gerenciador de Bibliotecas e selecione sua placa ESP32 e porta serial.
4. Grave o sketch na placa.
5. Envie a pasta `data/` usando o comando do plugin **Upload LittleFS**.
6. Abra o Monitor Serial em **115200 baud**. Acesse no navegador o endereço IP mostrado, por exemplo `http://192.168.0.123`.

O computador e o ESP32 precisam estar conectados à mesma rede Wi-Fi. O banco de dados é criado no LittleFS durante a primeira inicialização. Enviar uma nova imagem LittleFS substitui os arquivos armazenados na partição.

## API

A API local usa o prefixo `/smartvagas`. As rotas principais incluem `/estacionamentos`, `/vagas`, `/sensores` e `/leituras`. O site consulta essas rotas no próprio ESP32.

## Arquivos

- `smartvagas.ino`: configuração, sensores e inicialização.
- `banco.cpp`: dados JSON no LittleFS.
- `api.cpp`: rotas HTTP.
- `frontend.cpp`: entrega do site salvo no LittleFS.
- `data/`: HTML, CSS, JavaScript e imagens do site.
