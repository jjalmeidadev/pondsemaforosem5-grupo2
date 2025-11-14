# Semáforo Inteligente com ESP32 – Smart City

Esse projeto simula um sistema de semáforos inteligentes usando dois ESP32.
Um deles funciona como semáforo mestre, sendo responsável por decidir o estado do trânsito e enviar essas informações para o segundo ESP32, que funciona como semáforo secundário.

Os dispositivos se comunicam via MQTT, usam Wi-Fi, possuem modo noturno automático através de LDR e permitem controle manual através de uma página web hospedada pelo próprio mestre.

## Integrantes
Isaac Santos
Jaime Almeida
Guilherme Valenca
Victor Grycajuk
Richard Alves
Bruno Martins
Victor Santos

## Componentes utilizados

- 2x ESP32
- 6 LEDs (verde, amarelo, vermelho)
- Resistores de 220Ω
- 1 LDR + resistor 10kΩ (divisor de tensão)
- Jumpers e protoboard
- Cabo USB para alimentação e programação

## Funcionamento

Cada semáforo possui 3 LEDs. O LDR é ligado ao pino analógico do ESP32 mestre e o secundário recebe comandos via MQTT:

### Semáforo Mestre

- Lê o valor do LDR.
- Decide se está em modo diurno ou noturno.
- Controla seus próprios LEDs.
- Envia mensagens MQTT ao secundário:
"VERDE","AMARELO", "VERMELHO", "NOTURNO_ON", "NOTURNO_OFF"

### Semáforo Secundário

- Recebe mensagens do mestre via MQTT.
- Ajusta seus LEDs conforme a mensagem recebida.
- Pisca amarelo no modo noturno.

| Mensagem recebida | Ação executada no semáforo secundário |
|-------------------|----------------------------------------|
| VERDE         | Fica VERMELHO                     |
| AMARELO      | Fica AMARELO                      |
| VERMELHO     | Fica VERDE                       |
| NOTURNO_ON / NOTURNO_OFF | Pisca AMARELO           |

## Interface Web

O ESP32 mestre disponibiliza uma página acessível no navegador (IP - http://192.168.0.151/):

- Valor atual de luz
- Modo atual (diurno/noturno)
- Botão para ativar modo noturno
- Botão para voltar ao modo diurno

## Código Completo - Semáforo Mestre 
```
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Wi-Fi
const char* ssid = "Jaiminho23";
const char* password = "00000009";

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
```

## Código Completo - Semáforo Secundário 
```
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Wi-Fi
const char* ssid = "Jaiminho23";
const char* password = "00000009";

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
```

## Montagem Física e Interface
<div align="center">
<img width="745" height="545" alt="image" src="https://github.com/user-attachments/assets/5994069d-1eac-4d19-9b00-aec6c419bdd9" />
<img width="745" height="528" alt="image" src="https://github.com/user-attachments/assets/e00a8063-34a6-4518-8442-0078b66598a2" />
<img width="745" height="193" alt="image" src="https://github.com/user-attachments/assets/e96a08c2-b030-4303-aa40-fc82500493f0" />
</div>

## Funcionamento em vídeo

https://drive.google.com/file/d/1MqblOaBEcNL1MnC5oSLnEqgWMUCSy0AA/view?usp=drivesdk
