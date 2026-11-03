#include <Arduino.h>
#include <WiFi.h>
#include <Sensores.h>
#include <Actuadores.h>
#include <Pantalla.h>
#include <ClienteThingSpeak.h>
#include <BotControl.h>
#include "config.h"
#include "secrets.h"

Sensores           sensores(PIN_DHT, PIN_POTE);
Actuadores         actuadores(PIN_LED_VERDE, PIN_RELE);
Pantalla           pantalla;
ClienteThingSpeak  nube(TS_API_KEY, INTERVALO_THINGSPEAK_MS);
BotControl         bot(BOT_TOKEN, CHAT_ID_DEFAULT, sensores, actuadores, pantalla, nube, UMBRAL_VOLTAJE_V);

static void conectarWiFi() {
  pantalla.mostrarMensaje("Conectando WiFi...", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado. IP: " + WiFi.localIP().toString());
  pantalla.mostrarMensaje("WiFi conectado", WiFi.localIP().toString());
}

void setup() {
  Serial.begin(115200);
  sensores.begin();
  actuadores.begin();
  if (!pantalla.begin()) {
    Serial.println(F("Fallo al inicializar la OLED"));
    for (;;);
  }
  conectarWiFi();
  bot.begin();
}

void loop() {
  sensores.actualizar();

  // LED verde = temperatura dentro de lo normal
  actuadores.setLedVerde(sensores.hayLectura() && sensores.temperatura() <= UMBRAL_TEMP_C);

  // Rele con apagado automatico (no bloqueante)
  if (actuadores.actualizar()) bot.notificar("Relé apagado automáticamente (se cumplió el tiempo).");

  // Envio periodico a ThingSpeak
  if (nube.debeEnviar() && sensores.hayLectura()) {
    nube.enviar(sensores.temperatura(), sensores.humedad(), sensores.voltaje());
  }

  bot.actualizar();   // comandos de Telegram + alerta del potenciometro

  pantalla.mostrarEstado(sensores.temperatura(), sensores.humedad(), sensores.voltaje(),
                         sensores.hayLectura(), WiFi.status() == WL_CONNECTED);

  if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
}
