#pragma once
#include <Arduino.h>
#include <DHT.h>

// Lectura del DHT22 (cada 2 s) y del potenciometro (voltaje).
class Sensores {
public:
  Sensores(uint8_t pinDht, uint8_t pinPote);
  void begin();
  void actualizar();                 // llamar en cada loop()

  bool  hayLectura() const { return _hayLectura; }
  float temperatura() const { return _t; }
  float humedad() const { return _h; }
  float voltaje() const { return _v; }

private:
  float leerVoltaje();
  DHT _dht;
  uint8_t _pinPote;
  float _t, _h, _v;
  bool _hayLectura;
  unsigned long _ultimaLectura;
  static constexpr unsigned long INTERVALO_DHT_MS = 2000;
};
