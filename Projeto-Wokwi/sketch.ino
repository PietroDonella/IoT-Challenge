#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"
#include <SPI.h>
#include <MFRC522.h>

// Configurações Wi-Fi do Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurações MQTT (Broker Público para testes)
const char* mqtt_server = "broker.hivemq.com";
const char* topic_temp = "clyvovet/internacao/temp";
const char* topic_rfid = "clyvovet/triagem/rfid";

// Pinos
#define DHT_PIN 15
#define SS_PIN 5
#define RST_PIN 22

// Instâncias
DHTesp dht;
MFRC522 rfid(SS_PIN, RST_PIN);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("ESP32_ClyvoVet_Client")) {
      Serial.println("Conectado ao Broker MQTT!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializa Sensores
  dht.setup(DHT_PIN, DHTesp::DHT22);
  SPI.begin();
  rfid.PCD_Init();
  
  // Inicializa Redes
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  
  Serial.println("Sistema CLYVO VET Iniciado. Aproxime a Tag RFID do Pet...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 1. Leitura de Temperatura e Umidade
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) { // Lê a cada 5 segundos
    lastMsg = millis();
    float temp = dht.getTemperature();
    float hum = dht.getHumidity();
    
    if (!isnan(temp)) {
      String payload = "{\"temperatura\":" + String(temp) + ", \"umidade\":" + String(hum) + "}";
      client.publish(topic_temp, payload.c_str());
      Serial.println("Dados Ambientais Enviados: " + payload);
    }
  }

  // 2. Leitura do RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidString += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      uidString += String(rfid.uid.uidByte[i], HEX);
    }
    uidString.toUpperCase();
    
    String payload = "{\"pet_id\":\"" + uidString + "\"}";
    client.publish(topic_rfid, payload.c_str());
    Serial.println("Acesso Registrado! Pet ID: " + uidString);
    
    delay(2000); // Pausa para não floodar o leitor
  }
}