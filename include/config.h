#pragma once
#include <Arduino.h>

// ---- Pines (segun enunciado) ----
constexpr uint8_t PIN_DHT       = 33;
constexpr uint8_t PIN_POTE      = 32;  // ADC1
constexpr uint8_t PIN_LED_VERDE = 23;
constexpr uint8_t PIN_RELE      = 2;   // LED azul = rele / actuador principal

// ---- Umbrales y tiempos ----
constexpr float UMBRAL_TEMP_C     = 25.0;  // LED verde encendido si temp <= umbral
constexpr float UMBRAL_VOLTAJE_V  = 2.5;   // umbral critico del potenciometro
constexpr unsigned long INTERVALO_THINGSPEAK_MS = 20000; // minimo permitido: 15 s
