#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Sensores.h>
#include <Actuadores.h>
#include <Pantalla.h>
#include <ClienteThingSpeak.h>

// Bot de Telegram: /start (teclado inline), /clima, /rele, /thingup, /display<cmd>
// y alerta automatica cuando el potenciometro supera el umbral critico.
class BotControl {
public:
  BotControl(const char* token, const char* chatIdDefault,
             Sensores& sensores, Actuadores& actuadores,
             Pantalla& pantalla, ClienteThingSpeak& nube, float umbralVoltaje);

  void begin();
  void actualizar();                       // polling + chequeo de alerta
  void notificar(const String& texto);     // mensaje al ultimo chat conocido

private:
  void procesar(const String& chatId, String texto, const String& nombre);
  void enviarMenu(const String& chatId, const String& nombre);
  void cmdClima(const String& chatId);
  void cmdRele(const String& chatId, String args);
  void cmdThingup(const String& chatId);
  void cmdDisplay(const String& chatId, String cmd);
  void verificarAlerta();

  WiFiClientSecure _client;
  UniversalTelegramBot _bot;
  Sensores& _sensores;
  Actuadores& _act;
  Pantalla& _pantalla;
  ClienteThingSpeak& _nube;
  String _chatId;
  float _umbral;
  bool _alertaActiva;
  unsigned long _ultimoPolling;
  static constexpr unsigned long INTERVALO_POLLING_MS = 1000;
  static constexpr float HISTERESIS_V = 0.2;
};
