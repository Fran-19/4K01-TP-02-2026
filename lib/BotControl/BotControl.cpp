#include "BotControl.h"

BotControl::BotControl(const char* token, const char* chatIdDefault,
                       Sensores& sensores, Actuadores& actuadores,
                       Pantalla& pantalla, ClienteThingSpeak& nube, float umbralVoltaje)
  : _bot(token, _client), _sensores(sensores), _act(actuadores),
    _pantalla(pantalla), _nube(nube), _chatId(chatIdDefault),
    _umbral(umbralVoltaje), _alertaActiva(false), _ultimoPolling(0) {}

void BotControl::begin() {
  _client.setInsecure();   // sin validar certificado (suficiente para el TP)
  _bot.maxMessageLength = 8192;  // los botones traen el mensaje original completo
}
 
void BotControl::notificar(const String& texto) {
  if (_chatId.length() == 0) return;   // todavia no hay chat conocido
  _bot.sendMessage(_chatId, texto, "");
}

void BotControl::actualizar() {
  verificarAlerta();

  if (millis() - _ultimoPolling < INTERVALO_POLLING_MS) return;
  _ultimoPolling = millis();

  int n = _bot.getUpdates(_bot.last_message_received + 1);
  while (n > 0) {
    for (int i = 0; i < n; i++) {
      auto& m = _bot.messages[i];
      // Para mensajes normales y para botones inline (callback_query),
      // m.text trae el comando y m.chat_id el chat de origen.
      procesar(m.chat_id, m.text, m.from_name);
    }
    n = _bot.getUpdates(_bot.last_message_received + 1);
  }
}

void BotControl::procesar(const String& chatId, String texto, const String& nombre) {
  _chatId = chatId;            // las alertas automaticas van al ultimo chat activo
  texto.trim();

  // Quitar el sufijo "@NombreDelBot" si viene (chats grupales)
  int arroba = texto.indexOf('@');
  if (arroba > 0) {
    int espacio = texto.indexOf(' ', arroba);
    texto = (espacio < 0) ? texto.substring(0, arroba)
                          : texto.substring(0, arroba) + texto.substring(espacio);
  }
  texto.toLowerCase();

  if (texto == "/start")                 enviarMenu(chatId, nombre);
  else if (texto == "/clima")            cmdClima(chatId);
  else if (texto.startsWith("/rele"))    cmdRele(chatId, texto.substring(5));
  else if (texto == "/thingup")          cmdThingup(chatId);
  else if (texto.startsWith("/display")) cmdDisplay(chatId, texto.substring(8));
  else _bot.sendMessage(chatId, "Comando no reconocido. Usá /start para ver las opciones.", "");
}

void BotControl::enviarMenu(const String& chatId, const String& nombre) {
  String teclado = R"([
    [{"text":"Ver Clima","callback_data":"/clima"},
     {"text":"Ver Potenciómetro","callback_data":"/thingup"}],
    [{"text":"Encender Relé 10s","callback_data":"/rele on 10"},
     {"text":"Apagar Relé","callback_data":"/rele off"}],
    [{"text":"Ver Display LED","callback_data":"/displayled"},
     {"text":"Ver Display DHT22","callback_data":"/displaydht"}],
    [{"text":"Ver Display Pote","callback_data":"/displaypote"}]
  ])";
  String msg = "¡Bienvenido, " + nombre + "!\nUsá los botones de abajo.";
  _bot.sendMessageWithInlineKeyboard(chatId, msg, "", teclado);
}

void BotControl::cmdClima(const String& chatId) {
  if (!_sensores.hayLectura()) {
    _bot.sendMessage(chatId, "Todavía no hay lecturas válidas del DHT22.", "");
    return;
  }
  String msg = "Clima actual:\n- Temperatura: " + String(_sensores.temperatura(), 1) +
               " °C\n- Humedad: " + String(_sensores.humedad(), 1) + " %";
  _bot.sendMessage(chatId, msg, "");
}

void BotControl::cmdRele(const String& chatId, String args) {
  args.trim();
  String accion = args, tiempo = "";
  int sp = args.indexOf(' ');
  if (sp > 0) {
    accion = args.substring(0, sp);
    tiempo = args.substring(sp + 1);
    tiempo.trim();
  }

  if (accion == "on") {
    long seg = tiempo.toInt();
    if (tiempo.length() > 0 && seg <= 0) {
      _bot.sendMessage(chatId, "Tiempo inválido. Ejemplo: /rele on 10", "");
      return;
    }
    _act.releOn(seg);
    _bot.sendMessage(chatId, seg > 0 ? "Relé ENCENDIDO por " + String(seg) + " s."
                                     : "Relé ENCENDIDO (hasta /rele off).", "");
  } else if (accion == "off") {
    _act.releOff();
    _bot.sendMessage(chatId, "Relé APAGADO.", "");
  } else {
    _bot.sendMessage(chatId, "Uso: /rele on <segundos>  o  /rele off", "");
  }
}

void BotControl::cmdThingup(const String& chatId) {
  float v = _sensores.voltaje();
  float t = _sensores.hayLectura() ? _sensores.temperatura() : NAN;
  float h = _sensores.hayLectura() ? _sensores.humedad() : NAN;
  bool subido = _nube.enviar(t, h, v);

  String msg = "Potenciómetro: " + String(v, 2) + " V (umbral crítico: " +
               String(_umbral, 2) + " V)\n";
  msg += subido ? "Valor enviado a ThingSpeak." : "No se pudo enviar a ThingSpeak.";
  _bot.sendMessage(chatId, msg, "");
}

void BotControl::cmdDisplay(const String& chatId, String cmd) {
  cmd.trim();
  if (cmd == "led") {
    _pantalla.mostrarMensaje("LED verde:", _act.ledVerde() ? "Led encendido" : "Led apagado");
  } else if (cmd == "rele") {
    _pantalla.mostrarMensaje("Rele (LED azul):", _act.rele() ? "Rele encendido" : "Rele apagado");
  } else if (cmd == "pote") {
    _pantalla.mostrarMensaje("Potenciometro:", String(_sensores.voltaje(), 2) + " V");
  } else if (cmd == "dht") {
    if (_sensores.hayLectura())
      _pantalla.mostrarMensaje("DHT22:", "Temp: " + String(_sensores.temperatura(), 1) + " C",
                               "Hum: " + String(_sensores.humedad(), 1) + " %");
    else
      _pantalla.mostrarMensaje("DHT22:", "Sin lectura valida");
  } else {
    _pantalla.mostrarMensaje("Comando no", "identificado:", cmd.length() ? cmd : "(vacio)");
    _bot.sendMessage(chatId, "Comando de display no identificado: /display" + cmd, "");
    return;
  }
  _bot.sendMessage(chatId, "Estado mostrado en el display.", "");
}

// Alerta automatica: una sola vez al cruzar el umbral; se rearma al bajar (con histeresis).
void BotControl::verificarAlerta() {
  float v = _sensores.voltaje();
  if (!_alertaActiva && v > _umbral) {
    _alertaActiva = true;
    notificar("⚠️ ALERTA DE SEGURIDAD: el voltaje del potenciómetro es " + String(v, 2) +
              " V y superó el umbral crítico de " + String(_umbral, 2) + " V.");
  } else if (_alertaActiva && v < _umbral - HISTERESIS_V) {
    _alertaActiva = false;
  }
}
