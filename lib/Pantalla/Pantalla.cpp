#include "Pantalla.h"

Pantalla::Pantalla() : _oled(128, 64, &Wire, -1), _hastaMs(0), _ultimoDibujo(0) {}

bool Pantalla::begin() {
  if (!_oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) return false;
  _oled.clearDisplay();
  _oled.setTextColor(SSD1306_WHITE);
  _oled.setTextSize(1);
  _oled.display();
  return true;
}

void Pantalla::mostrarMensaje(const String& l1, const String& l2, const String& l3) {
  _oled.clearDisplay();
  _oled.setTextSize(1);
  _oled.setCursor(0, 0);
  _oled.println(l1);
  _oled.setCursor(0, 20);
  _oled.println(l2);
  _oled.setCursor(0, 40);
  _oled.println(l3);
  _oled.display();
  _hastaMs = millis() + DURACION_MENSAJE_MS;
}

void Pantalla::mostrarEstado(float t, float h, float v, bool hayLectura, bool wifiOk) {
  unsigned long ahora = millis();
  if (ahora < _hastaMs) return;                       // hay un mensaje en curso
  if (ahora - _ultimoDibujo < REFRESCO_MS) return;
  _ultimoDibujo = ahora;

  _oled.clearDisplay();
  _oled.setTextSize(1);
  _oled.setCursor(0, 0);
  _oled.println(wifiOk ? F("WiFi: OK") : F("WiFi: sin conexion"));
  _oled.setCursor(0, 16);
  if (hayLectura) {
    _oled.print(F("Temp: "));  _oled.print(t, 1); _oled.println(F(" C"));
    _oled.setCursor(0, 28);
    _oled.print(F("Hum:  "));  _oled.print(h, 1); _oled.println(F(" %"));
  } else {
    _oled.println(F("DHT22: sin lectura"));
  }
  _oled.setCursor(0, 40);
  _oled.print(F("Pote: ")); _oled.print(v, 2); _oled.println(F(" V"));
  _oled.display();
}
