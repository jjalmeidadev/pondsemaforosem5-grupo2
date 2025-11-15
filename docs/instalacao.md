# Guia de Instalação e Configuração

## Pré-requisitos

Antes de começar a trabalhar com este projeto, você precisará ter o Arduino IDE instalado em seu computador. A versão recomendada é a 2.0 ou superior, que oferece uma interface modernizada e melhor suporte para ESP32.

Além disso, certifique-se de ter os drivers USB para ESP32 instalados, pois eles são necessários para que seu computador reconheça a placa quando conectada via cabo USB.

## Instalação do Suporte ao ESP32

O Arduino IDE não vem com suporte nativo ao ESP32, então precisamos adicionar isso manualmente. Abra o Arduino IDE e navegue até `Arquivo > Preferências`. No campo `"URLs Adicionais para Gerenciadores de Placas"`, adicione a seguinte URL:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Após adicionar a URL, vá em Ferramentas → Placa → Gerenciador de Placas. Na janela que abrir, procure por "ESP32" e instale o pacote "esp32 by Espressif Systems". Este processo pode levar alguns minutos dependendo da velocidade da sua conexão.

## Instalação das Bibliotecas Necessárias

Este projeto utiliza a biblioteca PubSubClient para comunicação MQTT. Para instalá-la, vá em `Sketch > Incluir Biblioteca > Gerenciar Bibliotecas`. Na janela do Gerenciador de Bibliotecas, procure por "PubSubClient" e instale a versão de Nick O'Leary.

As demais bibliotecas usadas no projeto (WiFi, WebServer, WiFiClientSecure) já vêm incluídas no pacote de suporte ao ESP32 que instalamos anteriormente.

## Abrindo os Projetos

### Semáforo Mestre

Para abrir o projeto do semáforo mestre, navegue até a pasta `src/esp32-mestre/` e abra o arquivo `esp32-mestre.ino`. O Arduino IDE reconhecerá automaticamente este arquivo como um sketch devido à convenção de nomenclatura que seguimos (o arquivo .ino está dentro de uma pasta com o mesmo nome).

### Semáforo Secundário

Similarmente, para o semáforo secundário, navegue até `src/esp32-secundario/` e abra o arquivo `esp32-secundario.ino`.

## Configuração da Placa

Antes de fazer o upload do código, você precisa configurar a placa corretamente. Em `Ferramentas > Placa`, selecione "ESP32 Dev Module". Também verifique se a porta COM correta está selecionada em `Ferramentas > Porta` (ela geralmente aparece como "COMx" no Windows ou "/dev/ttyUSBx" no Linux).

## Configuração de Rede

Se você estiver usando uma rede WiFi diferente da configurada no código, será necessário alterar as credenciais. Abra o arquivo .ino correspondente e modifique as seguintes linhas no início do código:

```cpp
const char* ssid = "SEU_SSID_AQUI";
const char* password = "SUA_SENHA_AQUI";
```

**Importante:** Para evitar expor suas credenciais em repositórios públicos, considere criar um arquivo separado chamado `credentials.h` com essas informações e incluí-lo usando `#include "credentials.h"`. Não se esqueça de adicionar este arquivo ao `.gitignore`.

## Upload do Código

Com tudo configurado, você pode fazer o upload do código para o ESP32. Conecte a placa via USB, selecione a porta correta e clique no botão de upload (seta para a direita) na barra de ferramentas do Arduino IDE.

Durante o processo de upload, você pode precisar manter pressionado o botão "BOOT" na placa ESP32 até que a mensagem "Connecting..." apareça no console. Após isso, o código será transferido automaticamente.

## Monitoramento Serial

Para acompanhar o funcionamento do sistema e fazer debug, abra o Monitor Serial em `Ferramentas > Monitor Serial` e configure a velocidade para 115200 baud. Você verá mensagens sobre o status de conexão WiFi, MQTT e os comandos sendo enviados/recebidos.

## Solução de Problemas Comuns

Se você tiver problemas com a conexão MQTT, verifique se as credenciais do broker estão corretas e se seu firewall não está bloqueando a porta 8883. Caso o ESP32 não conecte ao WiFi, confirme se o SSID e senha estão digitados corretamente e se você está dentro do alcance da rede.

Para problemas relacionados à detecção da porta USB, tente reinstalar os drivers USB para ESP32 específicos do seu sistema operacional.