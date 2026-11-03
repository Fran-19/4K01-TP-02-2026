#include "Actuadores.h"

Actuadores::Actuadores(uint8_t pinLedVerde, uint8_t pinRele)
  : _pinLedVerde(pinLedVerde), _pinRele(pinRele),
    _ledVerde(false), _releOn(false), _inicioMs(0), _duracionMs(0) {}

void Actuadores::begin() {
  pinMode(_pinLedVerde, OUTPUT);
  pinMode(_pinRele, OUTPUT);
  digitalWrite(_pinLedVerde, LOW);
  digitalWrite(_pinRele, LOW);
}

void Actuadores::setLedVerde(bool encendido) {
  _ledVerde = encendido;
  digitalWrite(_pinLedVerde, encendido ? HIGH : LOW);
}

void Actuadores::releOn(unsigned long segundos) {
  _releOn = true;
  _inicioMs = millis();
  _duracionMs = segundos * 1000UL;
  digitalWrite(_pinRele, HIGH);
}

void Actuadores::releOff() {
  _releOn = false;
  _duracionMs = 0;
  digitalWrite(_pinRele, LOW);
}

unsigned long Actuadores::releSegundosRestantes() const {
  if (!_releOn || _duracionMs == 0) return 0;
  unsigned long transcurrido = millis() - _inicioMs;
  return transcurrido >= _duracionMs ? 0 : (_duracionMs - transcurrido) / 1000UL;
}

bool Actuadores::actualizar() {
  if (_releOn && _duracionMs > 0 && millis() - _inicioMs >= _duracionMs) {
    releOff();
    return true;
  }
  return false;
}
