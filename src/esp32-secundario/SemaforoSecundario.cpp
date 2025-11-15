#include "SemaforoSecundario.h"

// Construtor: inicializa os pinos
SemaforoSecundario::SemaforoSecundario(int v, int a, int r) 
  : verde(v), amarelo(a), vermelho(r) {
  piscaAtivo = false;
  estadoPisca = false;
  ultimaPiscada = 0;
  intervaloPisca = 500;  // Valor padrão
}

// Configura os pinos como saída
void SemaforoSecundario::begin() {
  pinMode(verde, OUTPUT);
  pinMode(amarelo, OUTPUT);
  pinMode(vermelho, OUTPUT);
  
  // Inicia com todos os LEDs desligados
  desligarTodos();
}

// Define o intervalo do pisca noturno
void SemaforoSecundario::setIntervaloPisca(unsigned long intervalo) {
  intervaloPisca = intervalo;
}

// Desliga todos os LEDs
void SemaforoSecundario::desligarTodos() {
  digitalWrite(verde, LOW);
  digitalWrite(amarelo, LOW);
  digitalWrite(vermelho, LOW);
}

// Ativa LED verde (semáforo secundário fica verde quando mestre está vermelho)
void SemaforoSecundario::modoSecundarioVerde() {
  piscaAtivo = false;  // Desativa pisca se estiver ativo
  desligarTodos();
  digitalWrite(verde, HIGH);
}

// Ativa LED amarelo
void SemaforoSecundario::modoSecundarioAmarelo() {
  piscaAtivo = false;  // Desativa pisca se estiver ativo
  desligarTodos();
  digitalWrite(amarelo, HIGH);
}

// Ativa LED vermelho (semáforo secundário fica vermelho quando mestre está verde)
void SemaforoSecundario::modoSecundarioVermelho() {
  piscaAtivo = false;  // Desativa pisca se estiver ativo
  desligarTodos();
  digitalWrite(vermelho, HIGH);
}

// Ativa o modo de pisca noturno
void SemaforoSecundario::modoNoturnoPiscar() {
  piscaAtivo = true;
  ultimaPiscada = millis();
}

// Atualiza o estado do pisca (deve ser chamado no loop)
void SemaforoSecundario::atualizar() {
  if (piscaAtivo) {
    unsigned long tempoAtual = millis();
    
    // Verifica se é hora de alternar o estado do LED
    if (tempoAtual - ultimaPiscada >= intervaloPisca) {
      ultimaPiscada = tempoAtual;
      estadoPisca = !estadoPisca;  // Alterna o estado
      
      desligarTodos();
      if (estadoPisca) {
        digitalWrite(amarelo, HIGH);
      }
    }
  }
}