#include <WiFi.h>
#include <WebServer.h>
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
const char* topicPub = "cidade/semaforo/mestre";

WiFiClientSecure secureClient;
PubSubClient client(secureClient);
WebServer server(80);


// Classe Semáforo Mestre 

class SemaforoMestre {
public:
  int verde;
  int amarelo;
  int vermelho;
  int ldr;

  int valorLuz = 0;
  bool modoNoturno = false;

  SemaforoMestre(int v, int a, int r, int l) {
    verde = v;
    amarelo = a;
    vermelho = r;
    ldr = l;
  }

  void iniciar() {
    pinMode(verde, OUTPUT);
    pinMode(amarelo, OUTPUT);
    pinMode(vermelho, OUTPUT);
    pinMode(ldr, INPUT);
  }

  void atualizarLuz() {
    valorLuz = analogRead(ldr);
    if (valorLuz < 1000) modoNoturno = true;
    else modoNoturno = false;
  }

  void modoDiurno() {
    digitalWrite(verde, HIGH);
    digitalWrite(amarelo, LOW);
    digitalWrite(vermelho, LOW);
    client.publish(topicPub, "VERDE");
    delay(3000);

    digitalWrite(verde, LOW);
    digitalWrite(amarelo, HIGH);
    client.publish(topicPub, "AMARELO");
    delay(1000);

    digitalWrite(amarelo, LOW);
    digitalWrite(vermelho, HIGH);
    client.publish(topicPub, "VERMELHO");
    delay(3000);

    digitalWrite(vermelho, LOW);
  }

  void modoNoturnoPiscar() {
    digitalWrite(amarelo, HIGH);
    client.publish(topicPub, "NOTURNO_ON");
    delay(500);

    digitalWrite(amarelo, LOW);
    client.publish(topicPub, "NOTURNO_OFF");
    delay(500);
  }
};

// instancia
SemaforoMestre semaforo(2, 4, 5, 34);

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
    if (client.connect("ESP32Mestre", mqttUser, mqttPassword)) {
      Serial.println("OK");
    } else {
      Serial.print("Falha, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

String gerarPagina() {
  String html = R"rawliteral(
  <html><body style='text-align:center;font-family:Arial;'>
  <h1>Semáforo Inteligente - Mestre</h1>
  <p>Luz: )rawliteral" + String(semaforo.valorLuz) + R"rawliteral(</p>
  <p>Modo: )rawliteral" + (semaforo.modoNoturno ? "Noturno" : "Diurno") + R"rawliteral(</p>
  <button onclick="location.href='/modoNoturnoOn'">Modo Noturno</button>
  <button onclick="location.href='/modoNoturnoOff'">Modo Diurno</button>
  </body></html>)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", gerarPagina());
}

void setup() {
  Serial.begin(115200);

  secureClient.setInsecure();

  semaforo.iniciar(); // inicia pinos

  conectarWiFi();
  client.setServer(mqttServer, mqttPort);
  conectarMQTT();

  server.on("/", handleRoot);
  server.on("/modoNoturnoOn", []() {
    semaforo.modoNoturno = true;
    server.send(200, "text/html", gerarPagina());
  });
  server.on("/modoNoturnoOff", []() {
    semaforo.modoNoturno = false;
    server.send(200, "text/html", gerarPagina());
  });

  server.begin();

  Serial.println("Mestre pronto!");
}

void loop() {
  if (!client.connected()) conectarMQTT();
  client.loop();
  server.handleClient();

  semaforo.atualizarLuz();

  if (semaforo.modoNoturno) semaforo.modoNoturnoPiscar();
  else semaforo.modoDiurno();
}
