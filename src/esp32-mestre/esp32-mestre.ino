#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "config.h"
#include "SemaforoMestre.h"

// Objetos de rede
WiFiClient wifiClient;  
PubSubClient client(wifiClient);
WebServer server(WEB_SERVER_PORT);

// Instância do semáforo
SemaforoMestre semaforo(PIN_LED_VERDE, PIN_LED_AMARELO, PIN_LED_VERMELHO, PIN_LDR);

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
    if (client.connect("ESP32Mestre", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println(" OK");
    } else {
      Serial.print(" FALHOU (rc=");
      Serial.print(client.state());
      Serial.println(")");
      delay(2000);
    }
  }
}

// Gera HTML da página web
String gerarPagina() {
  String html = R"rawliteral(
  <html>
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      body { text-align: center; font-family: Arial, sans-serif; margin: 20px; }
      h1 { color: #333; }
      .info { font-size: 18px; margin: 10px; }
      button { font-size: 16px; padding: 10px 20px; margin: 5px; cursor: pointer; }
    </style>
  </head>
  <body>
    <h1>Semáforo Inteligente - Mestre</h1>
    <div class="info">Luminosidade: )rawliteral" + String(semaforo.valorLuz) + R"rawliteral(</div>
    <div class="info">Modo: )rawliteral" + (semaforo.modoNoturno ? "Noturno" : "Diurno") + R"rawliteral(</div>
    <div class="info">Controle: )rawliteral" + (semaforo.modoManual ? "Manual" : "Automático") + R"rawliteral(</div>
    <div>
      <button onclick="location.href='/modoNoturnoOn'">Ativar Modo Noturno</button>
      <button onclick="location.href='/modoNoturnoOff'">Modo Automático</button>
    </div>
  </body>
  </html>)rawliteral";
  return html;
}

// Handler da página principal
void handleRoot() {
  server.send(200, "text/html", gerarPagina());
}

// Handler para ativar modo noturno
void handleModoNoturnoOn() {
  semaforo.ativarModoNoturno();
  server.send(200, "text/html", gerarPagina());
}

// Handler para desativar modo noturno (volta ao automático)
void handleModoNoturnoOff() {
  semaforo.desativarModoNoturno();
  server.send(200, "text/html", gerarPagina());
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Semáforo Inteligente - Mestre ===");
  
  // Configura conexão segura (sem validação de certificado)
  secureClient.setInsecure();
  
  // Inicializa o semáforo
  semaforo.iniciar();
  semaforo.setTempos(TEMPO_VERDE, TEMPO_AMARELO, TEMPO_VERMELHO, TEMPO_PISCA_NOTURNO);
  
  // Conecta ao WiFi
  conectarWiFi();
  
  // Configura e conecta ao MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  semaforo.setMQTTClient(&client, MQTT_TOPIC_PUB);
  conectarMQTT();
  
  // Configura as rotas do servidor web
  server.on("/", handleRoot);
  server.on("/modoNoturnoOn", handleModoNoturnoOn);
  server.on("/modoNoturnoOff", handleModoNoturnoOff);
  
  server.begin();
  
  Serial.println("Sistema pronto!");
  Serial.print("Acesse: http://");
  Serial.println(WiFi.localIP());
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
  
  // Processa requisições HTTP
  server.handleClient();
  
  // Atualiza leitura do sensor
  semaforo.atualizarLuz();
  
  // Atualiza estado do semáforo (NÃO-BLOQUEANTE)
  semaforo.atualizar();
}
