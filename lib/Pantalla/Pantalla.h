#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED: pantalla base con las mediciones + mensajes temporales (/display...).
class Pantalla {
public:
  Pantalla();
  bool begin();

  // Muestra hasta 3 lineas durante unos segundos y luego vuelve a la pantalla base.
  void mostrarMensaje(const String& l1, const String& l2 = "", const String& l3 = "");
  void mostrarEstado(float t, float h, float v, bool hayLectura, bool wifiOk);

private:
  Adafruit_SSD1306 _oled;
  unsigned long _hastaMs, _ultimoDibujo;
  static constexpr unsigned long DURACION_MENSAJE_MS = 5000;
  static constexpr unsigned long REFRESCO_MS = 500;
};
