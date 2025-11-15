# Guia de Instalação e Configuração

> **Nota**: Este guia explica como configurar o ambiente de desenvolvimento e fazer upload do código para os ESP32. Para entender a estrutura completa do projeto e documentação detalhada do código, consulte [`documentacao.md`](./documentacao.md).

## Pré-requisitos

Antes de começar a trabalhar com este projeto, você precisará ter o Arduino IDE instalado em seu computador. A versão recomendada é a 2.0 ou superior, que oferece uma interface modernizada e melhor suporte para ESP32.

Além disso, certifique-se de ter os drivers USB para ESP32 instalados, pois eles são necessários para que seu computador reconheça a placa quando conectada via cabo USB.

## Instalação do Suporte ao ESP32

O Arduino IDE não vem com suporte nativo ao ESP32, então precisamos adicionar isso manualmente. Abra o Arduino IDE e navegue até `Arquivo > Preferências`. No campo `"URLs Adicionais para Gerenciadores de Placas"`, adicione a seguinte URL:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Após adicionar a URL, vá em `Ferramentas > Placa > Gerenciador de Placas`. Na janela que abrir, procure por "ESP32" e instale o pacote "esp32 by Espressif Systems". Este processo pode levar alguns minutos dependendo da velocidade da sua conexão.

## Instalação das Bibliotecas Necessárias

Este projeto utiliza a biblioteca PubSubClient para comunicação MQTT. Para instalá-la, vá em `Sketch > Incluir Biblioteca > Gerenciar Bibliotecas`. Na janela do Gerenciador de Bibliotecas, procure por "PubSubClient" e instale a versão de Nick O'Leary.

As demais bibliotecas usadas no projeto (WiFi, WebServer, WiFiClientSecure) já vêm incluídas no pacote de suporte ao ESP32 que instalamos anteriormente.

## Abrindo os Projetos

### Semáforo Mestre

Para abrir o projeto do semáforo mestre, navegue até a pasta `src/esp32-mestre/` e abra o arquivo `esp32-mestre.ino`. O Arduino IDE reconhecerá automaticamente este arquivo como um sketch devido à convenção de nomenclatura que seguimos (o arquivo .ino está dentro de uma pasta com o mesmo nome).

Ao abrir o projeto, você notará que o Arduino IDE carrega automaticamente todos os arquivos do mesmo diretório (config.h, SemaforoMestre.h, SemaforoMestre.cpp) em abas separadas, facilitando a navegação entre os diferentes componentes do código.

### Semáforo Secundário

Similarmente, para o semáforo secundário, navegue até `src/esp32-secundario/` e abra o arquivo `esp32-secundario.ino`. Os demais arquivos do projeto (config.h, SemaforoSecundario.h, SemaforoSecundario.cpp) serão carregados automaticamente.

## Configuração da Placa

Antes de fazer o upload do código, você precisa configurar a placa corretamente. Em `Ferramentas > Placa`, selecione "ESP32 Dev Module". Também verifique se a porta COM correta está selecionada em `Ferramentas > Porta` (ela geralmente aparece como "COMx" no Windows ou "/dev/ttyUSBx" no Linux).

## Configuração de Rede

As credenciais de rede agora estão centralizadas no arquivo `config.h` de cada projeto. Se você estiver usando uma rede WiFi ou broker MQTT diferentes dos configurados, abra o arquivo `config.h` e modifique as seguintes constantes:

```cpp
// Configurações de Wi-Fi
const char* WIFI_SSID = "SEU_SSID_AQUI";
const char* WIFI_PASSWORD = "SUA_SENHA_AQUI";

// Configurações de MQTT
const char* MQTT_SERVER = "seu.broker.mqtt.aqui";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "seu_usuario";
const char* MQTT_PASSWORD = "sua_senha";
```

**Importante:** Para evitar expor suas credenciais em repositórios públicos, considere criar um arquivo separado chamado `credentials.h` com essas informações e incluí-lo no `config.h` usando `#include "credentials.h"`. Não se esqueça de adicionar `credentials.h` ao `.gitignore`.

## Upload do Código

Com tudo configurado, você pode fazer o upload do código para o ESP32. Conecte a placa via USB, selecione a porta correta e clique no botão de upload (seta para a direita) na barra de ferramentas do Arduino IDE.

Durante o processo de upload, você pode precisar manter pressionado o botão "BOOT" na placa ESP32 até que a mensagem "Connecting..." apareça no console. Após isso, o código será transferido automaticamente.

## Monitoramento Serial

Para acompanhar o funcionamento do sistema e fazer debug, abra o Monitor Serial em `Ferramentas > Monitor Serial` e configure a velocidade para 115200 baud. Você verá mensagens detalhadas sobre:

- Status de conexão WiFi (incluindo endereço IP atribuído)
- Status de conexão MQTT
- Comandos sendo enviados (no mestre)
- Mensagens sendo recebidas (no secundário)
- Tentativas de reconexão automática caso haja perda de conexão

## Solução de Problemas Comuns

Se você tiver problemas com a conexão MQTT, verifique se as credenciais do broker estão corretas no arquivo `config.h` e se seu firewall não está bloqueando a porta 8883. Caso o ESP32 não conecte ao WiFi, confirme se o SSID e senha estão digitados corretamente e se você está dentro do alcance da rede.

Para problemas relacionados à detecção da porta USB, tente reinstalar os drivers USB para ESP32 específicos do seu sistema operacional.

Se o código não compilar, verifique se todos os arquivos (.ino, .h, .cpp, config.h) estão na mesma pasta do projeto e se a biblioteca PubSubClient foi instalada corretamente.