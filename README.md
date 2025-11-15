# Semáforo Inteligente - Smart City

<center>

![INTELI](https://img.shields.io/badge/Inteli-231C30?style=for-the-badge&label=Modulo%204&labelColor=FFFFFF)
<br>
![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)

</center>

Sistema de semáforos inteligentes usando ESP32 com comunicação MQTT, controle automático via sensor LDR e interface web.

## Sobre o Projeto

Este projeto simula um sistema de semáforos inteligentes para Smart Cities, utilizando dois microcontroladores ESP32 que se comunicam via protocolo MQTT. O semáforo mestre coordena o tráfego e envia comandos ao semáforo secundário, com modo noturno automático baseado em sensor de luminosidade.

## Características

- Comunicação entre semáforos via MQTT (HiveMQ)
- Detecção automática de luminosidade (modo diurno/noturno)
- Interface web para controle manual
- Sincronização de estados entre semáforos
- Modo noturno com amarelo piscante
- Controle não-bloqueante com reconexão automática WiFi/MQTT

## Tecnologias

- ESP32
- Arduino IDE
- MQTT (WiFi + TLS)
- Sensor LDR
- LEDs RGB

## Estrutura do Repositório

```
semaforo-inteligente/
├── src/                          # Código-fonte dos projetos
│   ├── esp32-mestre/            # Firmware do semáforo mestre
│   └── esp32-secundario/        # Firmware do semáforo secundário
├── docs/                         # Documentação técnica
│   ├── documentacao.md          # Documentação completa do sistema
│   └── instalacao.md            # Guia de instalação e configuração
├── assets/                       # Imagens e recursos visuais
├── .gitignore                    # Arquivos ignorados pelo Git
├── LICENSE                       # Licença MIT
└── README.md                     # Este arquivo
```

## Equipe

- Bruno Martins
- Guilherme Valenca
- Isaac Santos
- Jaime Almeida
- Richard Alves
- Victor Grycajuk
- Victor Santos

## Documentação

A documentação completa do projeto está disponível em [`docs/documentacao.md`](./docs/documentacao.md).

O guia de instalação e configuração encontra-se em [`docs/instalacao.md`](./docs/instalacao.md). 

## Licença

Projeto desenvolvido para fins educacionais no INTELI - Instituto de Tecnologia e Liderança.