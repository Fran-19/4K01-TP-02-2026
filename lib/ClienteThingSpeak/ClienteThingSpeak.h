#pragma once
#include <Arduino.h>

// Envio periodico a ThingSpeak por HTTP (field1=temp, field2=hum, field3=voltaje).
class ClienteThingSpeak {
public:
  ClienteThingSpeak(const char* apiKey, unsigned long intervaloMs);
  bool debeEnviar() const;
  // Los valores NAN no se envian.
  bool enviar(float temperatura, float humedad, float voltaje);

private:
  const char* _apiKey;
  unsigned long _intervaloMs, _ultimoEnvio;
};
