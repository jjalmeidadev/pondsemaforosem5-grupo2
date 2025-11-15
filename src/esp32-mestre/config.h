#ifndef CONFIG_H
#define CONFIG_H

// Configurações de Wi-Fi
const char* WIFI_SSID = "Inteli.Iot";
const char* WIFI_PASSWORD = "%(Yk(sxGMtvFEs.3";

// Configurações de MQTT
const char* MQTT_SERVER = "e5cf094371aa436eb006414574faf775.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "pondsem5";
const char* MQTT_PASSWORD = "Argos1234";
const char* MQTT_TOPIC_PUB = "cidade/semaforo/mestre";

// Pinos do semáforo mestre
const int PIN_LED_VERDE = 2;
const int PIN_LED_AMARELO = 4;
const int PIN_LED_VERMELHO = 5;
const int PIN_LDR = 34;

// Configurações do sensor LDR
const int THRESHOLD_LDR = 1000;  // Valor abaixo do qual é considerado noturno

// Tempos dos semáforos (em milissegundos)
const unsigned long TEMPO_VERDE = 3000;
const unsigned long TEMPO_AMARELO = 1000;
const unsigned long TEMPO_VERMELHO = 3000;
const unsigned long TEMPO_PISCA_NOTURNO = 500;

// Porta do servidor web
const int WEB_SERVER_PORT = 80;

#endif