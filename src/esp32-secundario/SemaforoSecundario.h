#ifndef SEMAFORO_SECUNDARIO_H
#define SEMAFORO_SECUNDARIO_H

#include <Arduino.h>

class SemaforoSecundario {
private:
  // Pinos dos LEDs
  int verde;
  int amarelo;
  int vermelho;
  
  // Controle do pisca noturno (não-bloqueante)
  bool piscaAtivo;
  bool estadoPisca;  // true = ligado, false = desligado
  unsigned long ultimaPiscada;
  unsigned long intervaloPisca;

public:
  // Construtor
  SemaforoSecundario(int v, int a, int r);
  
  // Configuração inicial
  void begin();
  void setIntervaloPisca(unsigned long intervalo);
  
  // Métodos para controle dos LEDs
  void modoSecundarioVerde();
  void modoSecundarioAmarelo();
  void modoSecundarioVermelho();
  void modoNoturnoPiscar();
  
  // Atualiza o pisca (deve ser chamado no loop)
  void atualizar();
  
private:
  // Desliga todos os LEDs
  void desligarTodos();
};

#endif