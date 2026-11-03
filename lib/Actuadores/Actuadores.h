#pragma once
#include <Arduino.h>

// LED verde (indicador) y rele (LED azul) con apagado temporizado NO bloqueante.
class Actuadores {
public:
  Actuadores(uint8_t pinLedVerde, uint8_t pinRele);
  void begin();

  void setLedVerde(bool encendido);
  bool ledVerde() const { return _ledVerde; }

  void releOn(unsigned long segundos = 0);   // 0 = hasta que se apague a mano
  void releOff();
  bool rele() const { return _releOn; }
  unsigned long releSegundosRestantes() const;

  // Devuelve true en el instante en que el rele se apaga solo por temporizador.
  bool actualizar();

private:
  uint8_t _pinLedVerde, _pinRele;
  bool _ledVerde, _releOn;
  unsigned long _inicioMs, _duracionMs;
};
