# Documentação Técnica - Semáforo Inteligente

## Visão Geral

Este projeto simula um sistema de semáforos inteligentes usando dois ESP32. Um deles funciona como semáforo mestre, sendo responsável por decidir o estado do trânsito e enviar essas informações para o segundo ESP32, que funciona como semáforo secundário.

Os dispositivos se comunicam via MQTT, usam Wi-Fi, possuem modo noturno automático através de LDR e permitem controle manual através de uma página web hospedada pelo próprio mestre.

## Componentes Utilizados

- 2x ESP32
- 6 LEDs (verde, amarelo, vermelho)
- Resistores de 220Ω
- 1 LDR + resistor 10kΩ (divisor de tensão)
- Jumpers e protoboard
- Cabo USB para alimentação e programação

## Arquitetura do Sistema

### Semáforo Mestre

O semáforo mestre é responsável por coordenar todo o sistema. Ele executa as seguintes funções:

- Lê o valor do LDR para detectar luminosidade ambiente
- Decide automaticamente se está em modo diurno ou noturno
- Controla seus próprios LEDs de acordo com o modo ativo
- Envia mensagens MQTT ao semáforo secundário com os seguintes comandos: "VERDE", "AMARELO", "VERMELHO", "NOTURNO_ON", "NOTURNO_OFF"
- Disponibiliza interface web para controle manual

### Semáforo Secundário

O semáforo secundário atua como receptor das instruções do mestre:

- Recebe mensagens do mestre via protocolo MQTT
- Ajusta seus LEDs conforme a mensagem recebida
- Opera em modo complementar ao mestre (quando mestre está verde, secundário fica vermelho)
- Pisca amarelo durante o modo noturno

## Tabela de Comportamentos

| Mensagem Recebida | Ação Executada no Semáforo Secundário |
|-------------------|---------------------------------------|
| VERDE             | Fica VERMELHO                         |
| AMARELO           | Fica AMARELO                          |
| VERMELHO          | Fica VERDE                            |
| NOTURNO_ON        | Pisca AMARELO                         |
| NOTURNO_OFF       | Pisca AMARELO                         |

## Interface Web

O ESP32 mestre disponibiliza uma página web acessível através do seu endereço IP local (exemplo: http://192.168.0.151/). A interface oferece:

- Visualização do valor atual de luz captado pelo sensor LDR
- Indicação do modo atual (diurno ou noturno)
- Botão para forçar ativação do modo noturno
- Botão para retornar ao modo diurno

## Configuração de Rede

### WiFi

- SSID: Inteli.Iot
- Conexão via WPA2

### MQTT

- Broker: HiveMQ Cloud (TLS/SSL na porta 8883)
- Tópico de publicação (mestre): cidade/semaforo/mestre
- Autenticação via usuário e senha
- Conexão segura (TLS)

## Modos de Operação

### Modo Diurno

No modo diurno, o semáforo mestre executa o ciclo completo de trânsito:

1. Verde por 3 segundos
2. Amarelo por 1 segundo
3. Vermelho por 3 segundos
4. Retorna ao verde (ciclo contínuo)

Durante cada transição, uma mensagem correspondente é enviada ao semáforo secundário via MQTT.

### Modo Noturno

O modo noturno é ativado automaticamente quando o valor lido pelo LDR cai abaixo de 1000 (indicando baixa luminosidade). Neste modo:

- O LED amarelo do mestre pisca alternadamente (500ms ligado, 500ms desligado)
- Mensagens NOTURNO_ON e NOTURNO_OFF são alternadas via MQTT
- O semáforo secundário também pisca seu LED amarelo em sincronia
- O modo pode ser ativado ou desativado manualmente através da interface web

## Diagrama de Montagem

Consulte as imagens na seção "Montagem Física" para visualizar o circuito completo, incluindo:

- Conexões dos LEDs com resistores limitadores
- Montagem do divisor de tensão com LDR
- Pinagem dos ESP32
- Layout na protoboard

## Demonstração em Vídeo

Acesse o vídeo de funcionamento completo do sistema através do link:
https://drive.google.com/file/d/1MqblOaBEcNL1MnC5oSLnEqgWMUCSy0AA/view?usp=drivesdk

## Requisitos de Software

### Bibliotecas Necessárias

**Para o ESP32 Mestre:**
- WiFi.h (nativa ESP32)
- WebServer.h (nativa ESP32)
- WiFiClientSecure.h (nativa ESP32)
- PubSubClient.h (instalação via Library Manager)

**Para o ESP32 Secundário:**
- WiFi.h (nativa ESP32)
- WiFiClientSecure.h (nativa ESP32)
- PubSubClient.h (instalação via Library Manager)

### Instalação via Arduino IDE

1. Abra o Arduino IDE
2. Vá em Sketch → Include Library → Manage Libraries
3. Procure por "PubSubClient" e instale a biblioteca de Nick O'Leary
4. Configure a placa para ESP32 Dev Module em Tools → Board

## Segurança

⚠️ **Importante**: Este projeto contém credenciais de WiFi e MQTT no código-fonte para fins didáticos. Em um ambiente de produção, essas informações devem ser armazenadas de forma segura (por exemplo, em arquivos separados não versionados ou usando variáveis de ambiente).

Recomenda-se criar um arquivo `credentials.h` separado e adicioná-lo ao `.gitignore` para evitar exposição de dados sensíveis.