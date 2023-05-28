#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define LED_STATUS 18
#define LED_ALARMA 21
#define PIR 32
// Reemplazar con sus datos de su red WIFI
const char* ssid = "Name";
const char* password = "password";
// Inicializar BOT Telegram
#define BOTtoken "#########"  // Tu Bot Token (Obtener de Botfather)
// Usa @myidbot para averiguar el chat ID
// Tambien necesita hacer click en "start" antes de  enviarle mensajes el bot
#define CHAT_ID "0000000000"
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
  pinMode(LED_ALARMA, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, HIGH);
  // Configure el pin PIR como interrupción, asigne la función de interrupción y configure el modo RISING
  attachInterrupt(digitalPinToInterrupt(PIR), detectsMovement, RISING);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
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
    bot.sendMessage(CHAT_ID, "Movimiento detectado!!", "");
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_ALARMA, HIGH); // Enciende el LED
    delay(500);
    digitalWrite(LED_ALARMA, LOW); // Apaga el LED
    delay(500);
  }
    motionDetected = false;
  }
}