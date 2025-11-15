# Documentação Técnica - Semáforo Inteligente

## Sumário

1. [Visão Geral](#1-visão-geral)
2. [Componentes Utilizados](#2-componentes-utilizados)
3. [Arquitetura do Sistema](#3-arquitetura-do-sistema)
   - 3.1 [Semáforo Mestre](#31-semáforo-mestre)
   - 3.2 [Semáforo Secundário](#32-semáforo-secundário)
   - 3.3 [Tabela de Comportamentos](#33-tabela-de-comportamentos)
4. [Estrutura de Arquivos](#4-estrutura-de-arquivos)
   - 4.1 [ESP32 Mestre](#41-esp32-mestre)
   - 4.2 [ESP32 Secundário](#42-esp32-secundário)
5. [Documentação do Código](#5-documentação-do-código)
   - 5.1 [Conceitos Fundamentais](#51-conceitos-fundamentais)
   - 5.2 [Código do Semáforo Mestre](#52-código-do-semáforo-mestre)
   - 5.3 [Código do Semáforo Secundário](#53-código-do-semáforo-secundário)
6. [Interface Web](#6-interface-web)
7. [Configuração de Rede](#7-configuração-de-rede)
8. [Modos de Operação](#8-modos-de-operação)
9. [Diagrama de Montagem](#9-diagrama-de-montagem)
10. [Demonstração em Vídeo](#10-demonstração-em-vídeo)
11. [Requisitos de Software](#11-requisitos-de-software)
12. [Segurança](#12-segurança)

---

## 1. Visão Geral

Este projeto simula um sistema de semáforos inteligentes usando dois ESP32. Um deles funciona como semáforo mestre, sendo responsável por decidir o estado do trânsito e enviar essas informações para o segundo ESP32, que funciona como semáforo secundário.

Os dispositivos se comunicam via MQTT, usam Wi-Fi, possuem modo noturno automático através de LDR e permitem controle manual através de uma página web hospedada pelo próprio mestre. O código foi desenvolvido seguindo boas práticas de programação, com controle não-bloqueante e reconexão automática de rede.

## 2. Componentes Utilizados

Para montar este sistema de semáforos inteligentes, você precisará dos seguintes componentes eletrônicos:

- **2x ESP32**: Microcontroladores principais que executam toda a lógica do sistema
- **6 LEDs**: Dois conjuntos de três LEDs (verde, amarelo, vermelho) para representar os semáforos
- **Resistores de 220Ω**: Limitam a corrente dos LEDs para proteção
- **1 LDR + resistor 10kΩ**: Sensor de luminosidade em divisor de tensão para detectar luz ambiente
- **Jumpers e protoboard**: Para montagem e conexões
- **Cabo USB**: Para alimentação e programação dos ESP32

## 3. Arquitetura do Sistema

O sistema funciona com uma arquitetura mestre-escravo, onde o semáforo mestre toma todas as decisões e comunica-se com o semáforo secundário através do protocolo MQTT.

### 3.1. Semáforo Mestre

O semáforo mestre é responsável por coordenar todo o sistema. Ele executa as seguintes funções principais:

- **Leitura do sensor**: Lê continuamente o valor do LDR para detectar luminosidade ambiente
- **Decisão automática**: Decide automaticamente se deve operar em modo diurno ou noturno baseado na luminosidade
- **Controle local**: Controla seus próprios LEDs de acordo com o modo ativo utilizando máquina de estados
- **Comunicação MQTT**: Envia mensagens ao semáforo secundário com os comandos: "VERDE", "AMARELO", "VERMELHO", "NOTURNO_ON", "NOTURNO_OFF"
- **Interface web**: Disponibiliza servidor HTTP para controle manual e monitoramento
- **Reconexão automática**: Verifica e reconecta WiFi e MQTT automaticamente se a conexão cair

### 3.2. Semáforo Secundário

O semáforo secundário atua como receptor das instruções do mestre, executando as seguintes funções:

- **Recepção MQTT**: Recebe mensagens do mestre via protocolo MQTT através de callback
- **Controle complementar**: Ajusta seus LEDs conforme a mensagem recebida, operando de forma complementar ao mestre
- **Modo noturno sincronizado**: Pisca amarelo em sincronia com o mestre durante modo noturno
- **Reconexão automática**: Mantém a conexão WiFi e MQTT estável com tentativas automáticas de reconexão

### 3.3. Tabela de Comportamentos

A tabela abaixo mostra como o semáforo secundário reage a cada mensagem recebida do mestre:

| Mensagem Recebida | Ação Executada no Semáforo Secundário |
|-------------------|---------------------------------------|
| VERDE             | Fica VERMELHO                         |
| AMARELO           | Fica AMARELO                          |
| VERMELHO          | Fica VERDE                            |
| NOTURNO_ON        | Pisca AMARELO                         |
| NOTURNO_OFF       | Pisca AMARELO                         |

Note que os estados são complementares: quando o mestre está verde (veículos podem passar), o secundário fica vermelho (veículos devem parar), simulando um cruzamento real.

## 4. Estrutura de Arquivos

O projeto está organizado de forma modular, separando configurações, classes e lógica principal em arquivos distintos para facilitar manutenção e compreensão.

### 4.1. ESP32 Mestre

```
src/esp32-mestre/
├── esp32-mestre.ino          # Arquivo principal com setup() e loop()
├── config.h                  # Configurações centralizadas (WiFi, MQTT, pinos, tempos)
├── SemaforoMestre.h          # Declaração da classe SemaforoMestre
└── SemaforoMestre.cpp        # Implementação da classe SemaforoMestre
```

**Descrição dos arquivos:**

- **esp32-mestre.ino**: Arquivo principal que inicializa todos os componentes, configura conexões de rede, define as rotas do servidor web e executa o loop principal não-bloqueante. Gerencia a comunicação entre os diferentes módulos do sistema.

- **config.h**: Centraliza todas as constantes de configuração do sistema, incluindo credenciais WiFi e MQTT, definição de pinos dos LEDs e sensor, tempos de cada estado do semáforo e porta do servidor web. Facilita ajustes sem precisar modificar o código principal.

- **SemaforoMestre.h**: Header que declara a interface pública da classe SemaforoMestre, incluindo enumeração de estados possíveis, métodos públicos para controle e variáveis de estado. Define o contrato da classe.

- **SemaforoMestre.cpp**: Implementa toda a lógica de controle do semáforo usando máquina de estados não-bloqueante baseada em `millis()`. Controla transições entre estados (verde, amarelo, vermelho, noturno), gerencia LEDs e publica mensagens MQTT.

### 4.2. ESP32 Secundário

```
src/esp32-secundario/
├── esp32-secundario.ino      # Arquivo principal com setup() e loop()
├── config.h                  # Configurações centralizadas (WiFi, MQTT, pinos)
├── SemaforoSecundario.h      # Declaração da classe SemaforoSecundario
└── SemaforoSecundario.cpp    # Implementação da classe SemaforoSecundario
```

**Descrição dos arquivos:**

- **esp32-secundario.ino**: Arquivo principal que configura conexões de rede, define callback MQTT para processar mensagens recebidas e executa loop principal. Mais simples que o mestre pois não tem servidor web nem sensor.

- **config.h**: Centraliza configurações específicas do secundário, incluindo credenciais de rede e definição dos pinos dos LEDs. Estrutura similar ao config.h do mestre para manter consistência.

- **SemaforoSecundario.h**: Header que declara a classe SemaforoSecundario com métodos para controle de LEDs e modo de pisca noturno. Interface mais simples que o mestre pois apenas reage a comandos.

- **SemaforoSecundario.cpp**: Implementa o controle dos LEDs com lógica complementar ao mestre e pisca noturno não-bloqueante. Usa técnica similar de controle baseado em `millis()` para não travar o processamento de mensagens MQTT.

## 5. Documentação do Código

Esta seção explica detalhadamente como o código funciona, os conceitos por trás das decisões técnicas e como cada componente se integra no sistema completo.

### 5.1. Conceitos Fundamentais

Antes de mergulharmos no código, é importante entender alguns conceitos fundamentais que guiaram o desenvolvimento deste projeto.

#### Programação Não-Bloqueante

Em sistemas embarcados como o ESP32, temos múltiplas tarefas que precisam acontecer simultaneamente: ler sensores, processar mensagens de rede, responder a requisições web e controlar LEDs. Se usarmos a função `delay()` para criar pausas no código, o microcontrolador fica completamente parado durante esse tempo, incapaz de fazer qualquer outra coisa. Imagine tentar falar ao telefone enquanto está com os ouvidos tampados - você não consegue ouvir a outra pessoa porque está bloqueado fazendo outra coisa.

A solução é usar a função `millis()`, que retorna o número de milissegundos desde que o ESP32 foi ligado. Podemos guardar esse valor quando iniciamos uma ação e, em cada iteração do loop, verificar quanto tempo passou. Assim, o microcontrolador continua livre para executar outras tarefas enquanto "conta" o tempo. É como olhar para um relógio ocasionalmente em vez de ficar parado esperando o tempo passar.

#### Máquina de Estados

Uma máquina de estados é uma forma de organizar o comportamento do programa dividindo-o em estados distintos. No caso do semáforo, temos estados como VERDE, AMARELO e VERMELHO. Em cada momento, o semáforo está em exatamente um desses estados, e existem regras que determinam quando e como ele transita de um estado para outro. Por exemplo, após estar VERDE por três segundos, o semáforo deve mudar para AMARELO.

Essa abordagem torna o código muito mais claro e fácil de entender, pois cada comportamento está claramente associado a um estado específico. Também facilita adicionar novos estados ou modificar os existentes sem afetar o resto do código.

#### Separação de Responsabilidades

O código está organizado em diferentes arquivos e classes, cada um com uma responsabilidade específica. O arquivo `config.h` cuida apenas das configurações. A classe `SemaforoMestre` cuida apenas do controle do semáforo. O arquivo principal `.ino` cuida apenas de conectar tudo e gerenciar a comunicação de rede.

Essa separação torna o código mais fácil de entender porque você não precisa olhar tudo de uma vez. Se quiser mudar as credenciais WiFi, você sabe que precisa ir no `config.h`. Se quiser entender como o semáforo decide quando mudar de cor, você vai na classe `SemaforoMestre`. É como organizar uma casa em cômodos - cada espaço tem sua função específica.

### 5.2. Código do Semáforo Mestre

Vamos agora explorar cada componente do código do semáforo mestre, entendendo não apenas o que cada parte faz, mas por que foi feita dessa forma.

#### Arquivo config.h - Centralizando Configurações

Este arquivo reúne todas as constantes de configuração do sistema em um único lugar. A vantagem dessa abordagem é que se você precisar mudar algo - por exemplo, trocar de rede WiFi ou ajustar o tempo que o semáforo fica verde - você sabe exatamente onde ir. Além disso, usar constantes nomeadas torna o código muito mais legível do que ter números "mágicos" espalhados pelo programa.

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// As diretivas #ifndef/#define/#endif garantem que este arquivo
// seja incluído apenas uma vez durante a compilação, evitando
// erros de redefinição de constantes

// Configurações de Wi-Fi
// Estas constantes armazenam as credenciais da rede WiFi
const char* WIFI_SSID = "Inteli.Iot";
const char* WIFI_PASSWORD = "%(Yk(sxGMtvFEs.3";

// Configurações de MQTT
// O protocolo MQTT permite comunicação leve entre dispositivos IoT
// Aqui definimos o endereço do broker (servidor MQTT) e as credenciais
const char* MQTT_SERVER = "e5cf094371aa436eb006414574faf775.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;  // Porta 8883 indica conexão segura (TLS)
const char* MQTT_USER = "pondsem5";
const char* MQTT_PASSWORD = "Argos1234";
const char* MQTT_TOPIC_PUB = "cidade/semaforo/mestre";

// Pinos do semáforo mestre
// Definimos quais pinos GPIO do ESP32 estão conectados a cada LED
// Usar constantes facilita manutenção - se mudarmos o hardware,
// só precisamos alterar aqui
const int PIN_LED_VERDE = 2;
const int PIN_LED_AMARELO = 4;
const int PIN_LED_VERMELHO = 5;
const int PIN_LDR = 34;  // Pino analógico para leitura do sensor de luz

// Configurações do sensor LDR
// O threshold (limiar) define o ponto de corte entre dia e noite
// Valores abaixo de 1000 indicam pouca luz (modo noturno)
const int THRESHOLD_LDR = 1000;

// Tempos dos semáforos (em milissegundos)
// Definir os tempos como constantes facilita ajustes e torna o código
// mais legível do que usar números diretamente
const unsigned long TEMPO_VERDE = 3000;      // 3 segundos
const unsigned long TEMPO_AMARELO = 1000;    // 1 segundo
const unsigned long TEMPO_VERMELHO = 3000;   // 3 segundos
const unsigned long TEMPO_PISCA_NOTURNO = 500;  // 0.5 segundos

// Porta do servidor web
const int WEB_SERVER_PORT = 80;  // Porta padrão HTTP

#endif
```

#### Classe SemaforoMestre - Estrutura e Declarações

O arquivo de header (`SemaforoMestre.h`) define a estrutura da classe sem implementar os métodos. Isso segue o padrão C++ de separar interface (o que a classe faz) da implementação (como ela faz).

```cpp
#ifndef SEMAFORO_MESTRE_H
#define SEMAFORO_MESTRE_H

#include <Arduino.h>
#include <PubSubClient.h>

// Enumeração dos estados possíveis do semáforo
// Uma enumeração cria um tipo personalizado com valores nomeados
// Isso torna o código muito mais legível do que usar números (0, 1, 2...)
enum EstadoSemaforo {
  VERDE,         // Estado 0: LED verde ligado
  AMARELO,       // Estado 1: LED amarelo ligado
  VERMELHO,      // Estado 2: LED vermelho ligado
  NOTURNO_ON,    // Estado 3: Modo noturno com LED ligado
  NOTURNO_OFF    // Estado 4: Modo noturno com LED desligado
};

class SemaforoMestre {
private:
  // Atributos privados - só podem ser acessados dentro da classe
  // Isso protege os dados internos de modificações acidentais
  
  // Pinos dos LEDs e sensor
  // Guardamos quais pinos estão conectados a cada componente
  int verde;
  int amarelo;
  int vermelho;
  int ldr;
  
  // Estado do semáforo
  // Guardamos o estado atual usando a enumeração que definimos
  EstadoSemaforo estadoAtual;
  
  // Timestamp da última mudança de estado
  // Usamos unsigned long porque millis() pode chegar a valores muito grandes
  // (até 49 dias antes de resetar)
  unsigned long ultimaMudanca;
  
  // Referência ao cliente MQTT
  // Usamos ponteiro porque o objeto PubSubClient é criado fora da classe
  // e apenas passamos uma referência para ela
  PubSubClient* mqttClient;
  const char* topicPub;
  
  // Tempos de cada estado (em milissegundos)
  // Guardamos os tempos como variáveis para permitir configuração dinâmica
  unsigned long tempoVerde;
  unsigned long tempoAmarelo;
  unsigned long tempoVermelho;
  unsigned long tempoPisca;

public:
  // Atributos públicos - podem ser acessados de fora da classe
  // Escolhemos tornar alguns atributos públicos porque outras partes
  // do programa precisam ler esses valores (especialmente a interface web)
  
  int valorLuz;        // Valor atual lido do sensor LDR
  bool modoNoturno;    // Indica se está em modo noturno
  bool modoManual;     // Indica se o modo foi ativado manualmente pelo usuário
  
  // Construtor - método chamado quando criamos uma instância da classe
  // Recebe os pinos que serão usados para cada componente
  SemaforoMestre(int v, int a, int r, int l);
  
  // Métodos de configuração inicial
  void iniciar();  // Configura os pinos como entrada ou saída
  void setMQTTClient(PubSubClient* client, const char* topic);  // Define cliente MQTT
  void setTempos(unsigned long tVerde, unsigned long tAmarelo, 
                 unsigned long tVermelho, unsigned long tPisca);
  
  // Método de leitura do sensor
  void atualizarLuz();  // Lê o LDR e atualiza modoNoturno se necessário
  
  // Método principal de controle
  void atualizar();  // Atualiza o estado do semáforo baseado no tempo
  
  // Métodos de controle manual
  void ativarModoNoturno();    // Força modo noturno (desativa sensor)
  void desativarModoNoturno(); // Volta ao modo automático

private:
  // Métodos auxiliares privados - só usados internamente pela classe
  void mudarEstado(EstadoSemaforo novoEstado);  // Executa transição de estado
  void publicarMQTT(const char* mensagem);      // Envia mensagem via MQTT
};

#endif
```

#### Classe SemaforoMestre - Implementação Completa

Agora vamos ver como cada método é implementado. O arquivo `SemaforoMestre.cpp` contém toda a lógica de funcionamento.

```cpp
#include "SemaforoMestre.h"

// Construtor: inicializa todos os atributos quando criamos um objeto
// Recebe os pinos como parâmetros para tornar a classe flexível
SemaforoMestre::SemaforoMestre(int v, int a, int r, int l) {
  // Armazena os pinos que foram passados como parâmetro
  verde = v;
  amarelo = a;
  vermelho = r;
  ldr = l;
  
  // Inicializa os valores padrão das variáveis de estado
  valorLuz = 0;            // Ainda não lemos o sensor
  modoNoturno = false;     // Começa em modo diurno
  modoManual = false;      // Começa em modo automático (sensor ativo)
  estadoAtual = VERDE;     // Estado inicial é verde
  ultimaMudanca = 0;       // Ainda não houve mudança
  
  // Inicializa os ponteiros como nulos
  // Eles serão configurados depois através do método setMQTTClient
  mqttClient = nullptr;
  topicPub = nullptr;
}

// Configura os pinos como entrada ou saída
void SemaforoMestre::iniciar() {
  // Define os pinos dos LEDs como saída (OUTPUT)
  // Isso permite que o ESP32 controle o estado dos LEDs (ligar/desligar)
  pinMode(verde, OUTPUT);
  pinMode(amarelo, OUTPUT);
  pinMode(vermelho, OUTPUT);
  
  // Define o pino do LDR como entrada (INPUT)
  // Isso permite que o ESP32 leia o valor do sensor
  pinMode(ldr, INPUT);
  
  // Inicia com todos os LEDs desligados (LOW = 0V)
  // Isso garante um estado conhecido ao ligar o sistema
  digitalWrite(verde, LOW);
  digitalWrite(amarelo, LOW);
  digitalWrite(vermelho, LOW);
}

// Define a referência ao cliente MQTT
// Este método é chamado após criar o objeto PubSubClient no código principal
void SemaforoMestre::setMQTTClient(PubSubClient* client, const char* topic) {
  mqttClient = client;  // Guarda o ponteiro para o cliente MQTT
  topicPub = topic;     // Guarda o nome do tópico onde publicaremos mensagens
}

// Define os tempos de cada estado
// Permite configurar os tempos sem precisar recompilar o código
void SemaforoMestre::setTempos(unsigned long tVerde, unsigned long tAmarelo, 
                                unsigned long tVermelho, unsigned long tPisca) {
  tempoVerde = tVerde;
  tempoAmarelo = tAmarelo;
  tempoVermelho = tVermelho;
  tempoPisca = tPisca;
}

// Lê o sensor LDR e atualiza o modo (apenas se não estiver em modo manual)
void SemaforoMestre::atualizarLuz() {
  // Lê o valor analógico do LDR (0-4095 no ESP32)
  // Quanto mais luz, maior o valor lido
  valorLuz = analogRead(ldr);
  
  // Só atualiza o modo automaticamente se não estiver em modo manual
  // Se o usuário ativou manualmente o modo noturno via web,
  // não queremos que o sensor sobrescreva essa escolha
  if (!modoManual) {
    // Se o valor de luz está baixo (escuro), ativa modo noturno
    if (valorLuz < 1000) {
      modoNoturno = true;
    } else {
      // Caso contrário, desativa modo noturno
      modoNoturno = false;
    }
  }
}

// Ativa modo noturno manualmente
void SemaforoMestre::ativarModoNoturno() {
  modoNoturno = true;   // Ativa o modo noturno
  modoManual = true;    // Marca como controle manual
  // Agora o sensor LDR não alterará mais o modo automaticamente
}

// Desativa modo noturno e volta para controle automático
void SemaforoMestre::desativarModoNoturno() {
  modoNoturno = false;  // Desativa o modo noturno
  modoManual = false;   // Volta para controle automático
  // Agora o sensor LDR volta a controlar o modo baseado na luminosidade
}

// Método principal: atualiza o estado do semáforo
// Este método implementa a lógica não-bloqueante usando millis()
void SemaforoMestre::atualizar() {
  // Obtém o tempo atual em milissegundos desde que o ESP32 ligou
  unsigned long tempoAtual = millis();
  
  // Verifica se estamos em modo noturno
  if (modoNoturno) {
    // Modo noturno: alterna entre NOTURNO_ON e NOTURNO_OFF para criar efeito pisca
    
    if (estadoAtual == NOTURNO_ON) {
      // Se o LED está ligado, verifica se já passou tempo suficiente para desligar
      // Calculamos quanto tempo passou desde a última mudança
      if (tempoAtual - ultimaMudanca >= tempoPisca) {
        // Se passou o tempo configurado, desliga o LED
        mudarEstado(NOTURNO_OFF);
      }
      // Se não passou o tempo ainda, não faz nada e continua com LED ligado
    } else {
      // Se o LED está desligado, verifica se já passou tempo suficiente para ligar
      if (tempoAtual - ultimaMudanca >= tempoPisca) {
        // Se passou o tempo configurado, liga o LED
        mudarEstado(NOTURNO_ON);
      }
      // Se não passou o tempo ainda, não faz nada e continua com LED desligado
    }
  } else {
    // Modo diurno: executa o ciclo normal do semáforo (verde → amarelo → vermelho → verde...)
    
    // Usamos switch para verificar o estado atual e decidir a próxima ação
    switch (estadoAtual) {
      case VERDE:
        // Se estamos no verde, verificamos se já passou o tempo do verde
        if (tempoAtual - ultimaMudanca >= tempoVerde) {
          // Se sim, mudamos para amarelo
          mudarEstado(AMARELO);
        }
        // Se não, continuamos no verde e não fazemos nada
        break;
        
      case AMARELO:
        // Se estamos no amarelo, verificamos se já passou o tempo do amarelo
        if (tempoAtual - ultimaMudanca >= tempoAmarelo) {
          // Se sim, mudamos para vermelho
          mudarEstado(VERMELHO);
        }
        break;
        
      case VERMELHO:
        // Se estamos no vermelho, verificamos se já passou o tempo do vermelho
        if (tempoAtual - ultimaMudanca >= tempoVermelho) {
          // Se sim, voltamos para verde (completando o ciclo)
          mudarEstado(VERDE);
        }
        break;
        
      default:
        // Caso especial: se estávamos em modo noturno e voltamos para diurno,
        // o estado atual pode ser NOTURNO_ON ou NOTURNO_OFF
        // Nesse caso, iniciamos no verde
        mudarEstado(VERDE);
        break;
    }
  }
}

// Executa a transição para um novo estado
// Este método centraliza toda a lógica de mudança de estado
void SemaforoMestre::mudarEstado(EstadoSemaforo novoEstado) {
  // Atualiza o estado atual
  estadoAtual = novoEstado;
  
  // Registra o momento em que fizemos esta mudança
  // Isso é crucial para o controle de tempo não-bloqueante
  ultimaMudanca = millis();
  
  // Controla os LEDs físicos conforme o novo estado
  // Usamos switch para mapear cada estado aos comandos de hardware
  switch (estadoAtual) {
    case VERDE:
      // Estado verde: liga LED verde, desliga os outros
      digitalWrite(verde, HIGH);      // HIGH = 3.3V (ligado)
      digitalWrite(amarelo, LOW);     // LOW = 0V (desligado)
      digitalWrite(vermelho, LOW);
      // Publica mensagem MQTT para sincronizar o semáforo secundário
      publicarMQTT("VERDE");
      break;
      
    case AMARELO:
      // Estado amarelo: liga LED amarelo, desliga os outros
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, HIGH);
      digitalWrite(vermelho, LOW);
      publicarMQTT("AMARELO");
      break;
      
    case VERMELHO:
      // Estado vermelho: liga LED vermelho, desliga os outros
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, LOW);
      digitalWrite(vermelho, HIGH);
      publicarMQTT("VERMELHO");
      break;
      
    case NOTURNO_ON:
      // Modo noturno com LED ligado: apenas amarelo aceso
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, HIGH);
      digitalWrite(vermelho, LOW);
      publicarMQTT("NOTURNO_ON");
      break;
      
    case NOTURNO_OFF:
      // Modo noturno com LED desligado: todos apagados
      digitalWrite(verde, LOW);
      digitalWrite(amarelo, LOW);
      digitalWrite(vermelho, LOW);
      publicarMQTT("NOTURNO_OFF");
      break;
  }
}

// Publica uma mensagem via MQTT
void SemaforoMestre::publicarMQTT(const char* mensagem) {
  // Verifica se o cliente MQTT foi configurado
  // Esta verificação previne erros caso o método seja chamado
  // antes de configurarmos o cliente
  if (mqttClient != nullptr && topicPub != nullptr) {
    // Publica a mensagem no tópico configurado
    // O semáforo secundário está inscrito neste tópico e receberá a mensagem
    mqttClient->publish(topicPub, mensagem);
  }
}
```

#### Arquivo Principal - esp32-mestre.ino

O arquivo principal conecta todos os componentes e gerencia a comunicação de rede.

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "config.h"
#include "SemaforoMestre.h"

// Criação dos objetos de rede
// WiFiClientSecure permite conexão WiFi com criptografia TLS
WiFiClientSecure secureClient;

// PubSubClient gerencia a comunicação MQTT
// Passamos o secureClient para que use conexão segura
PubSubClient client(secureClient);

// WebServer cria um servidor HTTP na porta especificada
WebServer server(WEB_SERVER_PORT);

// Cria a instância do semáforo passando os pinos definidos em config.h
SemaforoMestre semaforo(PIN_LED_VERDE, PIN_LED_AMARELO, PIN_LED_VERMELHO, PIN_LDR);

// Variáveis de controle de reconexão WiFi
// Guardamos quando foi a última tentativa para não tentar reconectar a todo momento
unsigned long ultimaTentativaWiFi = 0;
const unsigned long INTERVALO_RECONEXAO_WIFI = 30000;  // 30 segundos entre tentativas

// Função que conecta ao WiFi
void conectarWiFi() {
  // Verifica se já está conectado para evitar tentativa desnecessária
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  
  // Inicia a conexão WiFi com as credenciais do config.h
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao WiFi");
  
  // Tenta conectar por até 10 segundos (20 tentativas × 500ms)
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);  // Pequeno delay é aceitável aqui pois é apenas no setup
    Serial.print(".");
    tentativas++;
  }
  
  // Informa o resultado da conexão
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());  // Mostra o IP atribuído
  } else {
    Serial.println(" FALHOU");
  }
}

// Função que verifica e reconecta WiFi se necessário
// Esta função é chamada no loop para manter a conexão estável
void verificarWiFi() {
  // Verifica se a conexão foi perdida
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long agora = millis();
    
    // Só tenta reconectar se já passou tempo suficiente desde a última tentativa
    // Isso evita sobrecarregar o sistema com tentativas constantes
    if (agora - ultimaTentativaWiFi >= INTERVALO_RECONEXAO_WIFI) {
      Serial.println("WiFi desconectado. Tentando reconectar...");
      conectarWiFi();
      ultimaTentativaWiFi = agora;
    }
  }
}

// Função que conecta ao broker MQTT
void conectarMQTT() {
  // Loop até conseguir conectar
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    
    // Tenta conectar com ID único e credenciais
    if (client.connect("ESP32Mestre", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println(" OK");
    } else {
      // Se falhou, mostra o código de erro
      // Códigos comuns: -2=falha de rede, -4=timeout, -5=falha de autenticação
      Serial.print(" FALHOU (rc=");
      Serial.print(client.state());
      Serial.println(")");
      delay(2000);  // Espera antes de tentar novamente
    }
  }
}

// Gera o HTML da interface web
String gerarPagina() {
  // Usa raw string literal (R"rawliteral(...)rawliteral") para facilitar
  // a escrita de HTML com múltiplas linhas e aspas
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
    <div class="info">💡 Luminosidade: )rawliteral" + String(semaforo.valorLuz) + R"rawliteral(</div>
    <div class="info">🌙 Modo: )rawliteral" + (semaforo.modoNoturno ? "Noturno" : "Diurno") + R"rawliteral(</div>
    <div class="info">🎛️ Controle: )rawliteral" + (semaforo.modoManual ? "Manual" : "Automático") + R"rawliteral(</div>
    <div>
      <button onclick="location.href='/modoNoturnoOn'">🌙 Ativar Modo Noturno</button>
      <button onclick="location.href='/modoNoturnoOff'">☀️ Modo Automático</button>
    </div>
  </body>
  </html>)rawliteral";
  return html;
}

// Handler (manipulador) da página principal
void handleRoot() {
  // Envia a página HTML como resposta HTTP
  server.send(200, "text/html", gerarPagina());
}

// Handler para ativar modo noturno manualmente
void handleModoNoturnoOn() {
  semaforo.ativarModoNoturno();  // Chama método da classe
  server.send(200, "text/html", gerarPagina());  // Envia página atualizada
}

// Handler para desativar modo noturno (volta ao automático)
void handleModoNoturnoOff() {
  semaforo.desativarModoNoturno();  // Chama método da classe
  server.send(200, "text/html", gerarPagina());  // Envia página atualizada
}

// Setup: executado uma vez quando o ESP32 liga
void setup() {
  // Inicia comunicação serial para debug
  Serial.begin(115200);
  Serial.println("\n=== Semáforo Inteligente - Mestre ===");
  
  // Configura conexão segura sem validação de certificado
  // Em produção, seria ideal validar o certificado do broker
  secureClient.setInsecure();
  
  // Inicializa o semáforo (configura pinos)
  semaforo.iniciar();
  
  // Configura os tempos do semáforo a partir do config.h
  semaforo.setTempos(TEMPO_VERDE, TEMPO_AMARELO, TEMPO_VERMELHO, TEMPO_PISCA_NOTURNO);
  
  // Conecta ao WiFi
  conectarWiFi();
  
  // Configura o servidor MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  
  // Passa a referência do cliente MQTT para o semáforo
  semaforo.setMQTTClient(&client, MQTT_TOPIC_PUB);
  
  // Conecta ao broker MQTT
  conectarMQTT();
  
  // Configura as rotas do servidor web
  // Quando alguém acessa "/" executa handleRoot
  server.on("/", handleRoot);
  server.on("/modoNoturnoOn", handleModoNoturnoOn);
  server.on("/modoNoturnoOff", handleModoNoturnoOff);
  
  // Inicia o servidor web
  server.begin();
  
  Serial.println("Sistema pronto!");
  Serial.print("Acesse: http://");
  Serial.println(WiFi.localIP());
}

// Loop: executado repetidamente enquanto o ESP32 estiver ligado
void loop() {
  // Verifica e reconecta WiFi se necessário
  // Importante manter a conexão WiFi estável
  verificarWiFi();
  
  // Verifica e reconecta MQTT se necessário
  // Só tenta se o WiFi estiver conectado (sem WiFi, MQTT não funciona)
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    conectarMQTT();
  }
  
  // Processa mensagens MQTT pendentes
  // Este método deve ser chamado frequentemente para manter a conexão
  client.loop();
  
  // Processa requisições HTTP do servidor web
  // Verifica se alguém acessou a página e processa a requisição
  server.handleClient();
  
  // Atualiza leitura do sensor LDR
  // Mantém o valor de luz sempre atualizado
  semaforo.atualizarLuz();
  
  // Atualiza estado do semáforo
  // Esta é a chamada mais importante - faz o semáforo funcionar
  semaforo.atualizar();
  
  // Note que não há delay() aqui - o loop executa milhares de vezes por segundo
  // permitindo que todas as tarefas sejam processadas rapidamente
}
```

### 5.3. Código do Semáforo Secundário

O semáforo secundário é mais simples que o mestre porque não possui sensor LDR nem servidor web. Sua principal função é receber comandos via MQTT e controlar os LEDs de forma complementar ao mestre.

#### Arquivo config.h do Secundário

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// Configurações de Wi-Fi
// Mesmas credenciais do mestre para conectar à mesma rede
const char* WIFI_SSID = "Inteli.Iot";
const char* WIFI_PASSWORD = "%(Yk(sxGMtvFEs.3";

// Configurações de MQTT
// Mesmo broker do mestre para permitir comunicação
const char* MQTT_SERVER = "e5cf094371aa436eb006414574faf775.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "pondsem5";
const char* MQTT_PASSWORD = "Argos1234";

// Tópico de inscrição - o mesmo onde o mestre publica
// Assim o secundário recebe todas as mensagens do mestre
const char* MQTT_TOPIC_SUB = "cidade/semaforo/mestre";

// Pinos do semáforo secundário
// Diferentes do mestre para evitar conflitos se ambos rodarem no mesmo ESP32
const int PIN_LED_VERDE = 18;
const int PIN_LED_AMARELO = 19;
const int PIN_LED_VERMELHO = 21;

// Tempo do pisca noturno (deve ser igual ao do mestre para sincronizar)
const unsigned long TEMPO_PISCA_NOTURNO = 500;

#endif
```

#### Classe SemaforoSecundario - Header

```cpp
#ifndef SEMAFORO_SECUNDARIO_H
#define SEMAFORO_SECUNDARIO_H

#include <Arduino.h>

class SemaforoSecundario {
private:
  // Pinos dos LEDs
  int verde;
  int amarelo;
  int vermelho;
  
  // Controle do pisca noturno não-bloqueante
  // Estas variáveis permitem criar o efeito de pisca sem usar delay()
  bool piscaAtivo;          // Indica se o modo pisca está ativo
  bool estadoPisca;         // Estado atual do pisca (ligado/desligado)
  unsigned long ultimaPiscada;  // Timestamp da última mudança no pisca
  unsigned long intervaloPisca; // Intervalo entre mudanças (em ms)

public:
  // Construtor
  SemaforoSecundario(int v, int a, int r);
  
  // Configuração inicial
  void begin();  // Configura pinos e estado inicial
  void setIntervaloPisca(unsigned long intervalo);  // Define tempo do pisca
  
  // Métodos para controle dos LEDs
  // Cada método corresponde a um possível estado do semáforo
  void modoSecundarioVerde();     // Liga LED verde
  void modoSecundarioAmarelo();   // Liga LED amarelo
  void modoSecundarioVermelho();  // Liga LED vermelho
  void modoNoturnoPiscar();       // Ativa modo pisca
  
  // Atualiza o pisca (deve ser chamado no loop)
  void atualizar();

private:
  // Método auxiliar privado
  void desligarTodos();  // Desliga todos os LEDs
};

#endif
```

#### Classe SemaforoSecundario - Implementação

```cpp
#include "SemaforoSecundario.h"

// Construtor usando lista de inicialização
// Esta sintaxe (: verde(v), amarelo(a)...) inicializa os atributos
// de forma mais eficiente que fazer dentro do corpo do construtor
SemaforoSecundario::SemaforoSecundario(int v, int a, int r) 
  : verde(v), amarelo(a), vermelho(r) {
  
  // Inicializa as variáveis de controle do pisca
  piscaAtivo = false;      // Começa com pisca desativado
  estadoPisca = false;     // Estado inicial do pisca
  ultimaPiscada = 0;       // Ainda não houve pisca
  intervaloPisca = 500;    // Valor padrão (será configurado depois)
}

// Configura os pinos e estado inicial
void SemaforoSecundario::begin() {
  // Define todos os pinos como saída
  pinMode(verde, OUTPUT);
  pinMode(amarelo, OUTPUT);
  pinMode(vermelho, OUTPUT);
  
  // Garante que todos os LEDs iniciam desligados
  desligarTodos();
}

// Configura o intervalo do pisca noturno
void SemaforoSecundario::setIntervaloPisca(unsigned long intervalo) {
  intervaloPisca = intervalo;
}

// Desliga todos os LEDs
// Método auxiliar usado por várias outras funções
void SemaforoSecundario::desligarTodos() {
  digitalWrite(verde, LOW);
  digitalWrite(amarelo, LOW);
  digitalWrite(vermelho, LOW);
}

// Ativa apenas o LED verde
// Este método é chamado quando o mestre está VERMELHO
// pois o secundário opera de forma complementar
void SemaforoSecundario::modoSecundarioVerde() {
  piscaAtivo = false;  // Desativa o modo pisca se estiver ativo
  desligarTodos();     // Desliga todos primeiro
  digitalWrite(verde, HIGH);  // Liga apenas o verde
}

// Ativa apenas o LED amarelo
void SemaforoSecundario::modoSecundarioAmarelo() {
  piscaAtivo = false;
  desligarTodos();
  digitalWrite(amarelo, HIGH);
}

// Ativa apenas o LED vermelho
// Este método é chamado quando o mestre está VERDE
void SemaforoSecundario::modoSecundarioVermelho() {
  piscaAtivo = false;
  desligarTodos();
  digitalWrite(vermelho, HIGH);
}

// Ativa o modo de pisca noturno
// Note que este método apenas ATIVA o pisca, não executa ele
// A execução acontece no método atualizar()
void SemaforoSecundario::modoNoturnoPiscar() {
  piscaAtivo = true;           // Marca que o pisca está ativo
  ultimaPiscada = millis();    // Registra o momento inicial
}

// Atualiza o estado do pisca (chamado repetidamente no loop)
// Este método implementa a lógica não-bloqueante do pisca
void SemaforoSecundario::atualizar() {
  // Só faz algo se o modo pisca estiver ativo
  if (piscaAtivo) {
    unsigned long tempoAtual = millis();
    
    // Verifica se já passou tempo suficiente desde a última mudança
    if (tempoAtual - ultimaPiscada >= intervaloPisca) {
      // Atualiza o timestamp
      ultimaPiscada = tempoAtual;
      
      // Inverte o estado (se estava ligado, desliga; se estava desligado, liga)
      estadoPisca = !estadoPisca;
      
      // Desliga todos os LEDs primeiro
      desligarTodos();
      
      // Se o novo estado é "ligado", acende o amarelo
      if (estadoPisca) {
        digitalWrite(amarelo, HIGH);
      }
      // Se o estado é "desligado", todos ficam apagados (já fizemos isso em desligarTodos)
    }
    // Se não passou o tempo ainda, não faz nada - esse é o segredo do não-bloqueante
  }
}
```

#### Arquivo Principal - esp32-secundario.ino

```cpp
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "config.h"
#include "SemaforoSecundario.h"

// Objetos de rede
WiFiClientSecure secureClient;
PubSubClient client(secureClient);

// Instância do semáforo com os pinos definidos em config.h
SemaforoSecundario semaforo(PIN_LED_VERDE, PIN_LED_AMARELO, PIN_LED_VERMELHO);

// Controle de reconexão WiFi
unsigned long ultimaTentativaWiFi = 0;
const unsigned long INTERVALO_RECONEXAO_WIFI = 30000;

// Conecta ao WiFi (mesma lógica do mestre)
void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
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
    
    // Usa ID diferente do mestre para evitar conflitos
    if (client.connect("ESP32Secundario", MQTT_USER, MQTT_PASSWORD)) {
      // Se conectou com sucesso, inscreve-se no tópico do mestre
      // A partir daqui, todas as mensagens publicadas pelo mestre
      // serão recebidas por este ESP32
      client.subscribe(MQTT_TOPIC_SUB);
      Serial.println(" OK");
    } else {
      Serial.print(" FALHOU (rc=");
      Serial.print(client.state());
      Serial.println(")");
      delay(2000);
    }
  }
}

// Callback MQTT: função chamada automaticamente quando uma mensagem chega
// Esta é a função mais importante do secundário - processa os comandos do mestre
void callback(char* topic, byte* payload, unsigned int length) {
  // Converte o payload (que vem como bytes) em String para facilitar comparação
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  
  // Mostra no Serial para debug
  Serial.print("Mensagem recebida: ");
  Serial.println(msg);
  
  // Processa os comandos com lógica INVERTIDA
  // Quando mestre envia "VERDE" (está verde), secundário fica vermelho
  // Isso simula um cruzamento real onde um lado passa enquanto o outro espera
  
  if (msg == "VERDE") {
    // Mestre verde → Secundário vermelho
    semaforo.modoSecundarioVermelho();
  } 
  else if (msg == "AMARELO") {
    // Mestre amarelo → Secundário amarelo (transição em ambos)
    semaforo.modoSecundarioAmarelo();
  } 
  else if (msg == "VERMELHO") {
    // Mestre vermelho → Secundário verde
    semaforo.modoSecundarioVerde();
  } 
  else if (msg == "NOTURNO_ON" || msg == "NOTURNO_OFF") {
    // Qualquer mensagem noturna → Ativa pisca
    // Note que recebemos tanto ON quanto OFF, mas não precisamos
    // alternar manualmente - o método atualizar() cuida disso
    semaforo.modoNoturnoPiscar();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Semáforo Inteligente - Secundário ===");
  
  // Inicializa o semáforo
  semaforo.begin();
  semaforo.setIntervaloPisca(TEMPO_PISCA_NOTURNO);
  
  // Configura conexão segura
  secureClient.setInsecure();
  
  // Conecta ao WiFi
  conectarWiFi();
  
  // Configura MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  
  // Define a função callback que será chamada quando mensagens chegarem
  client.setCallback(callback);
  
  // Conecta ao broker e inscreve no tópico
  conectarMQTT();
  
  Serial.println("Sistema pronto!");
}

void loop() {
  // Mantém conexão WiFi
  verificarWiFi();
  
  // Mantém conexão MQTT
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    conectarMQTT();
  }
  
  // Processa mensagens MQTT
  // Esta chamada é crucial - é ela que aciona o callback quando mensagens chegam
  client.loop();
  
  // Atualiza o pisca noturno se estiver ativo
  // Sem esta chamada, o pisca não funcionaria
  semaforo.atualizar();
}
```

## 6. Interface Web

O ESP32 mestre disponibiliza uma página web acessível através do seu endereço IP local. Para acessar, você precisa estar na mesma rede WiFi que o ESP32 e digitar o endereço IP mostrado no Monitor Serial quando o sistema inicializa (exemplo: http://192.168.0.151/).

A interface foi projetada para ser simples e intuitiva, oferecendo:

**Informações em Tempo Real:**
- Valor atual de luminosidade captado pelo sensor LDR (número de 0 a 4095)
- Modo de operação atual (Diurno ou Noturno)
- Tipo de controle ativo (Manual ou Automático)

**Controles:**
- Botão "Ativar Modo Noturno": força o semáforo a operar em modo noturno independentemente da luz ambiente. O sensor LDR é desativado e o sistema permanece no modo escolhido até que você o desative.
- Botão "Modo Automático": retorna ao controle automático baseado no sensor LDR. O sistema voltará a decidir sozinho quando entrar ou sair do modo noturno.

A interface utiliza HTML com CSS inline, garantindo que funcione em qualquer navegador sem necessidade de arquivos externos. O design é responsivo, adaptando-se a diferentes tamanhos de tela.

## 7. Configuração de Rede

### WiFi

O sistema utiliza WiFi para conectar ambos os ESP32 à rede local. A configuração atual usa:

- **SSID**: Inteli.Iot
- **Protocolo de Segurança**: WPA2
- **Reconexão Automática**: O sistema verifica a conexão a cada 30 segundos e tenta reconectar automaticamente se detectar que a conexão foi perdida

### MQTT

O protocolo MQTT é usado para comunicação entre os semáforos. MQTT é ideal para IoT porque é leve, confiável e funciona bem mesmo em redes com latência variável. A configuração usa:

- **Broker**: HiveMQ Cloud (serviço gerenciado de broker MQTT)
- **Protocolo**: MQTT sobre TLS/SSL para segurança
- **Porta**: 8883 (porta padrão para MQTT seguro)
- **Tópico**: cidade/semaforo/mestre
- **Autenticação**: Usuário e senha para acesso ao broker
- **Reconexão**: Automática quando detectada perda de conexão

O funcionamento é simples: o mestre publica mensagens no tópico, e o secundário está inscrito neste mesmo tópico, recebendo todas as mensagens automaticamente.

## 8. Modos de Operação

### Modo Diurno

No modo diurno, o sistema simula o funcionamento de um semáforo real de trânsito. O semáforo mestre executa um ciclo completo que se repete continuamente:

**Fase Verde (3 segundos)**: O LED verde do mestre acende, permitindo passagem de veículos na sua direção. Simultaneamente, o secundário recebe a mensagem "VERDE" via MQTT e acende seu LED vermelho, sinalizando que veículos na direção perpendicular devem parar.

**Fase Amarelo (1 segundo)**: O LED amarelo do mestre acende, indicando transição iminente. O secundário também acende seu amarelo, preparando ambas as direções para a mudança.

**Fase Vermelho (3 segundos)**: O LED vermelho do mestre acende, parando o tráfego na sua direção. O secundário recebe "VERMELHO" e acende seu verde, permitindo agora a passagem na direção perpendicular.

Após completar este ciclo, o sistema retorna automaticamente à fase verde, reiniciando o processo. Todo o controle é feito de forma não-bloqueante, permitindo que o sistema continue respondendo a comandos e processando outras tarefas durante o ciclo.

### Modo Noturno

O modo noturno pode ser ativado de duas formas distintas, oferecendo flexibilidade ao sistema:

**Ativação Automática**: O sensor LDR mede continuamente a luminosidade ambiente. Quando o valor lido cai abaixo de 1000 (em uma escala de 0 a 4095, onde valores mais altos indicam mais luz), o sistema interpreta que está escuro e ativa automaticamente o modo noturno. Isso acontece naturalmente ao anoitecer ou se o ambiente escurecer por qualquer motivo.

**Ativação Manual**: Através da interface web, você pode clicar no botão "Ativar Modo Noturno" para forçar o sistema a operar neste modo independentemente da luz ambiente. Isso é útil para testes ou situações especiais. Quando ativado manualmente, o sistema ignora as leituras do sensor LDR até que você clique em "Modo Automático" para retornar ao controle baseado em sensor.

**Comportamento no Modo Noturno**: Quando ativo, ambos os semáforos passam a piscar apenas o LED amarelo alternadamente. O LED fica ligado por 500 milissegundos, depois desligado por 500 milissegundos, repetindo este padrão continuamente. Este comportamento é sincronizado entre mestre e secundário através das mensagens MQTT "NOTURNO_ON" e "NOTURNO_OFF". O pisca amarelo intermitente é um padrão reconhecido internacionalmente para indicar aos motoristas que devem prosseguir com cautela.

## 9. Diagrama de Montagem

Para montar fisicamente este projeto, você precisa conectar os componentes seguindo algumas diretrizes importantes:

**Conexão dos LEDs**: Cada LED deve ser conectado a um pino GPIO do ESP32 através de um resistor de 220Ω. O resistor limita a corrente para proteger tanto o LED quanto o pino do microcontrolador. A conexão é: pino GPIO → resistor → ânodo do LED (perna mais longa) → cátodo do LED (perna mais curta) → GND (terra). Repita para os três LEDs de cada semáforo.

**Montagem do Sensor LDR**: O LDR funciona como divisor de tensão. Conecte uma perna do LDR ao 3.3V do ESP32 e a outra a um resistor de 10kΩ. A conexão entre o LDR e o resistor vai ao pino analógico 34. A outra perna do resistor vai ao GND. Esta configuração permite que o ESP32 leia a variação de resistência do LDR como uma variação de tensão.

**Pinagem Específica**: Para o mestre, use os pinos 2 (verde), 4 (amarelo), 5 (vermelho) e 34 (LDR). Para o secundário, use os pinos 18 (verde), 19 (amarelo) e 21 (vermelho). Estes pinos foram escolhidos porque são seguros para uso geral no ESP32.

Consulte as imagens de referência do projeto para visualizar a montagem completa na protoboard, facilitando a replicação do circuito.

## 10. Demonstração em Vídeo

Para ver o sistema funcionando na prática, acesse o vídeo de demonstração que mostra:

- O ciclo completo do semáforo em modo diurno com sincronização perfeita entre mestre e secundário
- A transição automática para modo noturno quando a luz ambiente diminui
- O uso da interface web para controle manual
- O comportamento de reconexão quando a rede é interrompida e restabelecida

**Link do vídeo**: https://drive.google.com/file/d/1MqblOaBEcNL1MnC5oSLnEqgWMUCSy0AA/view?usp=drivesdk

## 11. Requisitos de Software

### Bibliotecas Necessárias

Para compilar e executar este projeto, você precisa ter as seguintes bibliotecas instaladas no Arduino IDE:

**Para o ESP32 Mestre:**
- WiFi.h: biblioteca nativa do ESP32 para conexões WiFi
- WebServer.h: biblioteca nativa para criar servidor HTTP
- WiFiClientSecure.h: biblioteca nativa para conexões seguras (TLS/SSL)
- PubSubClient.h: biblioteca externa para comunicação MQTT (instalação via Library Manager)

**Para o ESP32 Secundário:**
- WiFi.h: biblioteca nativa do ESP32
- WiFiClientSecure.h: biblioteca nativa para conexões seguras
- PubSubClient.h: biblioteca externa para MQTT

### Instalação

As bibliotecas nativas (WiFi, WebServer, WiFiClientSecure) já vêm incluídas quando você instala o suporte para ESP32 no Arduino IDE. Você não precisa instalá-las separadamente.

A biblioteca PubSubClient precisa ser instalada manualmente. No Arduino IDE, vá em `Sketch > Include Library > Manage Libraries`, procure por "PubSubClient" e instale a versão de Nick O'Leary. Esta é a biblioteca mais popular para MQTT em Arduino e é bem mantida e documentada.

Para instruções completas e detalhadas sobre todo o processo de instalação, incluindo configuração do Arduino IDE e upload do código, consulte o guia em [`docs/instalacao.md`](./instalacao.md).

## 12. Segurança

### Aviso Importante sobre Credenciais

Este projeto contém credenciais de WiFi e MQTT diretamente no arquivo `config.h` para fins educacionais e facilitar o aprendizado. Em um ambiente de produção ou ao compartilhar seu código publicamente, esta prática não é recomendada pois expõe informações sensíveis.

### Boas Práticas Recomendadas

Para melhorar a segurança do projeto:

**Arquivo de Credenciais Separado**: Crie um arquivo `credentials.h` contendo apenas as credenciais:

```cpp
#ifndef CREDENTIALS_H
#define CREDENTIALS_H

const char* WIFI_SSID = "sua_rede";
const char* WIFI_PASSWORD = "sua_senha";
const char* MQTT_USER = "seu_usuario";
const char* MQTT_PASSWORD = "sua_senha_mqtt";

#endif
```

Depois, inclua este arquivo no `config.h` usando `#include "credentials.h"` e remova as definições duplicadas.

**Uso do .gitignore**: Adicione uma linha no arquivo `.gitignore` do projeto:
```
credentials.h
```

Isso garante que o arquivo com suas credenciais nunca seja enviado para repositórios Git públicos.

**Senhas Fortes**: Sempre use senhas fortes e únicas para suas conexões WiFi e MQTT. Evite senhas padrão ou facilmente adivinháveis.

**Validação de Certificados**: No código atual, usamos `secureClient.setInsecure()` que desabilita a validação de certificados TLS. Em produção, considere validar o certificado do broker MQTT para garantir que está se conectando ao servidor legítimo.

Seguindo estas práticas, você mantém seu sistema mais seguro sem comprometer a funcionalidade do projeto.