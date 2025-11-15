#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "config.h"
#include "SemaforoSecundario.h"

// Objetos de rede
WiFiClientSecure secureClient;
PubSubClient client(secureClient);

// Instância do semáforo
SemaforoSecundario semaforo(PIN_LED_VERDE, PIN_LED_AMARELO, PIN_LED_VERMELHO);

// Variáveis de controle de reconexão
unsigned long ultimaTentativaWiFi = 0;
const unsigned long INTERVALO_RECONEXAO_WIFI = 30000;  // Tenta reconectar a cada 30 segundos

// Conecta ao WiFi
void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;  // Já está conectado
  }
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao WiFi");
  
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" FALHOU");
  }
}

// Verifica e reconecta WiFi se necessário
void verificarWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long agora = millis();
    if (agora - ultimaTentativaWiFi >= INTERVALO_RECONEXAO_WIFI) {
      Serial.println("WiFi desconectado. Tentando reconectar...");
      conectarWiFi();
      ultimaTentativaWiFi = agora;
    }
  }
}

// Conecta ao broker MQTT
void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32Secundario", MQTT_USER, MQTT_PASSWORD)) {
      // Se conectou, inscreve-se no tópico
      client.subscribe(MQTT_TOPIC_SUB);
      Serial.println(" OK");
    } else {
      Serial.print(" FALHOU (rc=");
      Serial.print(client.state());
      Serial.println(")");
      delay(2000);
    }
  }
}

// Callback MQTT: processa mensagens recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  // Converte payload em String
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  
  Serial.print("Mensagem recebida: ");
  Serial.println(msg);
  
  // Processa os comandos
  // Lógica invertida: quando mestre está VERDE, secundário fica VERMELHO e vice-versa
  if (msg == "VERDE") {
    semaforo.modoSecundarioVermelho();
  } 
  else if (msg == "AMARELO") {
    semaforo.modoSecundarioAmarelo();
  } 
  else if (msg == "VERMELHO") {
    semaforo.modoSecundarioVerde();
  } 
  else if (msg == "NOTURNO_ON" || msg == "NOTURNO_OFF") {
    semaforo.modoNoturnoPiscar();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Semáforo Inteligente - Secundário ===");
  
  // Inicializa o semáforo
  semaforo.begin();
  semaforo.setIntervaloPisca(TEMPO_PISCA_NOTURNO);
  
  // Configura conexão segura (sem validação de certificado)
  secureClient.setInsecure();
  
  // Conecta ao WiFi
  conectarWiFi();
  
  // Configura e conecta ao MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  conectarMQTT();
  
  Serial.println("Sistema pronto!");
}

void loop() {
  // Verifica e reconecta WiFi se necessário
  verificarWiFi();
  
  // Verifica e reconecta MQTT se necessário
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    conectarMQTT();
  }
  
  // Processa mensagens MQTT
  client.loop();
  
  // Atualiza o pisca noturno (se estiver ativo)
  semaforo.atualizar();
}
