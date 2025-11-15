#include "SemaforoMestre.h"

// Construtor: inicializa os pinos
SemaforoMestre::SemaforoMestre(int v, int a, int r, int l) {
  verde = v;
  amarelo = a;
  vermelho = r;
  ldr = l;
  
  valorLuz = 0;
  modoNoturno = false;
  modoManual = false;  // Inicia no modo automático
  estadoAtual = VERDE;
  ultimaMudanca = 0;
  
  mqttClient = nullptr;
  topicPub = nullptr;
}

// Configura os pinos como entrada/saída
void SemaforoMestre::iniciar() {
  pinMode(verde, OUTPUT);
  pinMode(amarelo, OUTPUT);
  pinMode(vermelho, OUTPUT);
  pinMode(ldr, INPUT);
  
  // Inicia com todos os LEDs desligados
  digitalWrite(verde, LOW);
  digitalWrite(amarelo, LOW);
  digitalWrite(vermelho, LOW);
}

// Define a referência ao cliente MQTT
void SemaforoMestre::setMQTTClient(PubSubClient* client, const char* topic) {
  mqttClient = client;
  topicPub = topic;
}

// Define os tempos de cada estado
void SemaforoMestre::setTempos(unsigned long tVerde, unsigned long tAmarelo, unsigned long tVermelho, unsigned long tPisca) {
  tempoVerde = tVerde;
  tempoAmarelo = tAmarelo;
  tempoVermelho = tVermelho;
  tempoPisca = tPisca;
}

// Lê o sensor LDR e atualiza o modo (apenas se não estiver em modo manual)
void SemaforoMestre::atualizarLuz() {
  valorLuz = analogRead(ldr);
  
  // Só atualiza o modo automaticamente se não estiver em modo manual
  if (!modoManual) {
    if (valorLuz < 1000) {
      modoNoturno = true;
    } else {
      modoNoturno = false;
    }
  }
}

// Ativa modo noturno manualmente
void SemaforoMestre::ativarModoNoturno() {
  modoNoturno = true;
  modoManual = true;  // Marca como modo manual
}

// Desativa modo noturno (volta para automático)
void SemaforoMestre::desativarModoNoturno() {
  modoNoturno = false;
  modoManual = false;  // Volta para modo automático
}

// Método principal: atualiza o estado do semáforo (NÃO-BLOQUEANTE)
void SemaforoMestre::atualizar() {
  unsigned long tempoAtual = millis();
  
  if (modoNoturno) {
    // Modo noturno: pisca amarelo
    if (estadoAtual == NOTURNO_ON) {
      if (tempoAtual - ultimaMudanca >= tempoPisca) {
        mudarEstado(NOTURNO_OFF);
      }
    } else {
      if (tempoAtual - ultimaMudanca >= tempoPisca) {
        mudarEstado(NOTURNO_ON);
      }
    }
  } else {
    // Modo diurno: ciclo normal
    switch (estadoAtual) {
      case VERDE:
        if (tempoAtual - ultimaMudanca >= tempoVerde) {
          mudarEstado(AMARELO);
        }
        break;
        
      case AMARELO:
        if (tempoAtual - ultimaMudanca >= tempoAmarelo) {
          mudarEstado(VERMELHO);
        }
        break;
        
      case VERMELHO:
        if (tempoAtual - ultimaMudanca >= tempoVermelho) {
          mudarEstado(VERDE);
        }
        break;
        
      default:
        // Se estava em modo noturno e voltou para diurno, inicia no verde
        mudarEstado(VERDE);
        break;
    }
  }
}

// Muda o estado do semáforo e atualiza os LEDs
void SemaforoMestre::mudarEstado(EstadoSemaforo novoEstado) {
  estadoAtual = novoEstado;
  ultimaMudanca = millis();
  
  // Controla os LEDs conforme o estado
  switch (estadoAtual) {
    case VERDE:
      digitalWrite(verde, HIGH);
      digitalWrite(amarelo, LOW);
      digitalWrite(vermelho, LOW);
      publicarMQTT("VERDE");
      break;
      
    case AMARELO:
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, HIGH);
      digitalWrite(vermelho, LOW);
      publicarMQTT("AMARELO");
      break;
      
    case VERMELHO:
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, LOW);
      digitalWrite(vermelho, HIGH);
      publicarMQTT("VERMELHO");
      break;
      
    case NOTURNO_ON:
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, HIGH);
      digitalWrite(vermelho, LOW);
      publicarMQTT("NOTURNO_ON");
      break;
      
    case NOTURNO_OFF:
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, LOW);
      digitalWrite(vermelho, LOW);
      publicarMQTT("NOTURNO_OFF");
      break;
  }
}

// Publica mensagem via MQTT (se cliente estiver configurado)
void SemaforoMestre::publicarMQTT(const char* mensagem) {
  if (mqttClient != nullptr && topicPub != nullptr) {
    mqttClient->publish(topicPub, mensagem);
  }
}