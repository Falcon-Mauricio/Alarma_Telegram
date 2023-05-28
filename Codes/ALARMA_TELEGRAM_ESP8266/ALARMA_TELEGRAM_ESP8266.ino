#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define LED_STATUS 12
#define LED_WIFI 14
#define BUZZER 13
#define PIR 4

const char* ssid = "Nombre_de_tu_red";       // Nombre de tu red WiFi
const char* password = "Contraseña_de_tu_red"; // Contraseña de tu red WiFi

// Inicializar BOT Telegram
#define BOTtoken "Token"  // Tu Bot Token (Obtener de Botfather)
// Usa @myidbot para averiguar el chat ID
// Tambien necesita hacer click en "start" antes de  enviarle mensajes el bot
#define CHAT_ID "ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
bool motionDetected = false;

// Indica cuando se detecta movimiento
void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

void handleNewMessages(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Usuario No Autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;

    if (text == "/alarma_on") {
      bot.sendMessage(chat_id, "Alarma activada", "");
      digitalWrite(LED_STATUS, HIGH);
    }

    if (text == "/alarma_off") {
      bot.sendMessage(chat_id, "Alarma desactivada", "");
      digitalWrite(LED_STATUS, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Configura el pin PIR como entrada pull-up
  pinMode(PIR, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_WIFI, LOW);
  for (int i = 0; i < 2; i++) {
      digitalWrite(LED_STATUS, HIGH); // Enciende el LED
      delay(500);
      digitalWrite(LED_STATUS, LOW); // Apaga el LED
      delay(500);
    }
  // Configure el pin PIR como interrupción, asigne la función de interrupción y configure el modo RISING
  attachInterrupt(digitalPinToInterrupt(PIR), detectsMovement, RISING);
  Serial.print("Conectando a la red WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(LED_WIFI, LOW);
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  digitalWrite(LED_WIFI, HIGH);
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "Bot iniciado", "");
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  if (motionDetected) {
    bot.sendMessage(CHAT_ID, "¡Movimiento detectado!", "");
    for (int i = 0; i < 10; i++) {
      digitalWrite(BUZZER, HIGH); // Enciende el BUZZER
      delay(500);
      digitalWrite(BUZZER, LOW); // Apaga el BUZZER
      delay(500);
    }
    motionDetected = false;
  }
}