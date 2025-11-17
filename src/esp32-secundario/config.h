#ifndef CONFIG_H
#define CONFIG_H

// Configurações de Wi-Fi
const char* WIFI_SSID = "Inteli.Iot";
const char* WIFI_PASSWORD = "%(Yk(sxGMtvFEs.3";

// Configurações de MQTT
const char* MQTT_SERVER = "192.168.15.3";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "secundario";
const char* MQTT_PASSWORD = "r2d2";
const char* MQTT_TOPIC_SUB = "cidade/semaforo/mestre";

// Pinos do semáforo secundário
const int PIN_LED_VERDE = 18;
const int PIN_LED_AMARELO = 19;
const int PIN_LED_VERMELHO = 21;

// Tempo do pisca noturno (em milissegundos)
const unsigned long TEMPO_PISCA_NOTURNO = 500;

#endif