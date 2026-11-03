#include "ClienteThingSpeak.h"
#include <WiFi.h>
#include <HTTPClient.h>

ClienteThingSpeak::ClienteThingSpeak(const char* apiKey, unsigned long intervaloMs)
  : _apiKey(apiKey), _intervaloMs(intervaloMs), _ultimoEnvio(0) {}

bool ClienteThingSpeak::debeEnviar() const {
  return millis() - _ultimoEnvio >= _intervaloMs;
}

bool ClienteThingSpeak::enviar(float t, float h, float v) {
  _ultimoEnvio = millis();                       // evita reintentos en rafaga
  if (WiFi.status() != WL_CONNECTED) return false;

  String url = String("http://api.thingspeak.com/update?api_key=") + _apiKey;
  if (!isnan(t)) url += "&field1=" + String(t, 2);
  if (!isnan(h)) url += "&field2=" + String(h, 2);
  if (!isnan(v)) url += "&field3=" + String(v, 2);

  HTTPClient http;
  http.begin(url);
  int codigo = http.GET();
  String cuerpo = (codigo > 0) ? http.getString() : "";
  http.end();

  bool ok = (codigo == 200 && cuerpo.toInt() > 0);   // ThingSpeak devuelve 0 si falla
  Serial.printf("[ThingSpeak] HTTP %d, respuesta: %s\n", codigo, cuerpo.c_str());
  return ok;
}
