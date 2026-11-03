#include "Sensores.h"

Sensores::Sensores(uint8_t pinDht, uint8_t pinPote)
  : _dht(pinDht, DHT22), _pinPote(pinPote),
    _t(NAN), _h(NAN), _v(0), _hayLectura(false), _ultimaLectura(0) {}

void Sensores::begin() {
  _dht.begin();
  analogReadResolution(12);
  pinMode(_pinPote, INPUT);
}

float Sensores::leerVoltaje() {
  uint32_t suma = 0;
  for (int i = 0; i < 8; i++) suma += analogRead(_pinPote);
  return (suma / 8.0f) * 3.3f / 4095.0f;
}

void Sensores::actualizar() {
  _v = leerVoltaje();

  unsigned long ahora = millis();
  if (ahora - _ultimaLectura < INTERVALO_DHT_MS) return;
  _ultimaLectura = ahora;

  float h = _dht.readHumidity();
  float t = _dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("[Sensores] Error al leer el DHT22"));
    return;  // se conservan los ultimos valores validos
  }
  _h = h;
  _t = t;
  _hayLectura = true;
}
