#ifndef SEMAFORO_MESTRE_H
#define SEMAFORO_MESTRE_H

#include <Arduino.h>
#include <PubSubClient.h>

// Estados possíveis do semáforo
enum EstadoSemaforo {
  VERDE,
  AMARELO,
  VERMELHO,
  NOTURNO_ON,
  NOTURNO_OFF
};

class SemaforoMestre {
private:
  // Pinos dos LEDs e sensor
  int verde;
  int amarelo;
  int vermelho;
  int ldr;
  
  // Estado do semáforo
  EstadoSemaforo estadoAtual;
  unsigned long ultimaMudanca;  // Timestamp da última mudança de estado
  
  // Referência ao cliente MQTT
  PubSubClient* mqttClient;
  const char* topicPub;
  
  // Tempos de cada estado (ms)
  unsigned long tempoVerde;
  unsigned long tempoAmarelo;
  unsigned long tempoVermelho;
  unsigned long tempoPisca;

public:
  int valorLuz;
  bool modoNoturno;
  bool modoManual;  // Flag para distinguir modo manual do automático
  
  // Construtor
  SemaforoMestre(int v, int a, int r, int l);
  
  // Configuração inicial
  void iniciar();
  void setMQTTClient(PubSubClient* client, const char* topic);
  void setTempos(unsigned long tVerde, unsigned long tAmarelo, unsigned long tVermelho, unsigned long tPisca);
  
  // Leitura do sensor
  void atualizarLuz();
  
  // Controle do semáforo (não-bloqueante)
  void atualizar();
  
  // Controle manual do modo
  void ativarModoNoturno();
  void desativarModoNoturno();

private:
  // Métodos auxiliares para controle de estado
  void mudarEstado(EstadoSemaforo novoEstado);
  void publicarMQTT(const char* mensagem);
};

#endif