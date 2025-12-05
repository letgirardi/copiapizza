
#include <WiFi.h>
#include <PubSubClient.h>

// --- Configurações da sua Rede Wi-Fi ---
const char* ssid = "Redmi 13C";
const char* password = "Echidna12";

// --- Configurações do Broker MQTT ---
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// --- Configurações do Sensor ---
const int LDR_PIN_A0 = 34;  // Pino do LDR

// --- Configurações MQTT ---
const char* mqtt_client_id = "Melch";
const char* mqtt_topic_publish = "mel/LDR/Luminosidade";

// --- LEDs ---
const int LED_WIFI = 17;   // LED para status do Wi-Fi
const int LED_MQTT = 18;   // LED para status do MQTT

// --- Variáveis Globais ---
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
const int PUBLISH_INTERVAL = 5000; // 5 segundos

// --- FUNÇÕES DE CONEXÃO ---
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI, LOW); // Desliga LED enquanto conecta
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  digitalWrite(LED_WIFI, HIGH); // Liga LED quando conectado
}

void reconnect() {
  while (!client.connected()) {
    digitalWrite(LED_MQTT, LOW); // Desliga LED enquanto tenta conectar
    Serial.print("Tentando conexão MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("conectado!");
      digitalWrite(LED_MQTT, HIGH); // Liga LED quando conectado
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);

  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_MQTT, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  lastMsg = millis();
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > PUBLISH_INTERVAL) {
    lastMsg = now;

    int ldrValue = analogRead(LDR_PIN_A0);
    char msgBuffer[10];
    snprintf(msgBuffer, 10, "%d", ldrValue);

    Serial.print("Publicando no topico: ");
    Serial.print(mqtt_topic_publish);
    Serial.print(" -> Valor do LDR: ");
    Serial.println(msgBuffer);

    client.publish(mqtt_topic_publish, msgBuffer);
  }
}
