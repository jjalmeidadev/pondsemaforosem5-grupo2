# Semáforo Inteligente - Smart City

Sistema de semáforos inteligentes usando ESP32 com comunicação MQTT, controle automático via sensor LDR e interface web.

## Sobre o Projeto

Este projeto simula um sistema de semáforos inteligentes para Smart Cities, utilizando dois microcontroladores ESP32 que se comunicam via protocolo MQTT. O semáforo mestre coordena o tráfego e envia comandos ao semáforo secundário, com modo noturno automático baseado em sensor de luminosidade.

## Características

- Comunicação entre semáforos via MQTT (HiveMQ)
- Detecção automática de luminosidade (modo diurno/noturno)
- Interface web para controle manual
- Sincronização de estados entre semáforos
- Modo noturno com amarelo piscante

## Tecnologias

- ESP32
- Arduino IDE
- MQTT (WiFi + TLS)
- Sensor LDR
- LEDs RGB

## Equipe

- Bruno Martins
- Guilherme Valenca
- Isaac Santos
- Jaime Almeida
- Richard Alves
- Victor Grycajuk
- Victor Santos


## Documentação

A documentação completa do projeto está disponível em [`docs/`](./docs/).

## Licença

Projeto desenvolvido para fins educacionais no INTELI - Instituto de Tecnologia e Liderança.