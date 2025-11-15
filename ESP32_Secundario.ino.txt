#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Wi-Fi
const char* ssid = "Inteli.Iot";
const char* password = "%(Yk(sxGMtvFEs.3";

// MQTT
const char* mqttServer = "e5cf094371aa436eb006414574faf775.s1.eu.hivemq.cloud";
const int mqttPort = 8883;
const char* mqttUser = "pondsem5";
const char* mqttPassword = "Argos1234";
const char* topicSub = "cidade/semaforo/mestre";

// Classe Semáforo Secundário
class SemaforoSecundario {
public:
  int verde, amarelo, vermelho;

  SemaforoSecundario(int v, int a, int r)
      : verde(v), amarelo(a), vermelho(r) {}

  void begin() {
    pinMode(verde, OUTPUT);
    pinMode(amarelo, OUTPUT);
    pinMode(vermelho, OUTPUT);
  }

  void modoSecundarioVerde() {
    digitalWrite(vermelho, LOW);
    digitalWrite(amarelo, LOW);
    digitalWrite(verde, HIGH);
    delay(500);
  }

  void modoSecundarioAmarelo() {
    digitalWrite(verde, LOW);
    digitalWrite(vermelho, LOW);
    digitalWrite(amarelo, HIGH);
    delay(500);
  }

  void modoSecundarioVermelho() {
    digitalWrite(verde, LOW);
    digitalWrite(amarelo, LOW);
    digitalWrite(vermelho, HIGH);
    delay(500);
  }

  void modoNoturnoPiscar() {
    digitalWrite(verde, LOW);
    digitalWrite(vermelho, LOW);
    digitalWrite(amarelo, HIGH);
    delay(500);
    digitalWrite(amarelo, LOW);
    delay(500);
  }
};

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

// Instância dos LEDs 
SemaforoSecundario semaforo(18, 19, 21);

// Classe Rede e MQTT
class ComunicacaoMQTT {
public:
  void conectarWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("OK");
    Serial.println(WiFi.localIP());
  }

  void conectarMQTT() {
    while (!client.connected()) {
      Serial.print("MQTT...");
      if (client.connect("ESP32Secundario", mqttUser, mqttPassword)) {
        client.subscribe(topicSub);
        Serial.println("OK");
      } else {
        Serial.print("Falha, rc=");
        Serial.println(client.state());
        delay(2000);
      }
    }
  }
};

ComunicacaoMQTT net;

// Callback MQTT 
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.println("Msg: " + msg);

  if (msg == "VERDE") semaforo.modoSecundarioVermelho();
  else if (msg == "AMARELO") semaforo.modoSecundarioAmarelo();
  else if (msg == "VERMELHO") semaforo.modoSecundarioVerde();
  else if (msg == "NOTURNO_ON" || msg == "NOTURNO_OFF") semaforo.modoNoturnoPiscar();
}

void setup() {
  Serial.begin(115200);

  semaforo.begin();
  secureClient.setInsecure();

  net.conectarWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  net.conectarMQTT();

  Serial.println("Secundário pronto!");
}

void loop() {
  if (!client.connected()) net.conectarMQTT();
  client.loop();
}
