# CLYVO VET - Monitoramento IoT e Triagem Inteligente

## 👥 Integrantes
* **Enzo Vaz – RM: 561702
* **Lucas Ryuji Fukuda – RM: 562152
* **Pietro Donella Salomão – RM: 561722

## 📋 Descrição do Projeto e o Problema
Em clínicas veterinárias movimentadas, a troca de pacientes (o que pode causar medicação errada) e o estresse térmico em gaiolas de internação são riscos graves. 
Este projeto visa resolver esse problema criando um ecossistema de cuidado seguro para a CLYVO VET, garantindo a correta identificação dos animais e o monitoramento ambiental em tempo real durante internações e tratamentos.

## 💡 A Solução IoT
Cada pet recebe uma tag RFID única em sua coleira. Ao passar a tag no leitor do consultório ou da gaiola, o sistema identifica o animal instantaneamente, funcionando como a "chave" de acesso ao seu prontuário digital. 
Simultaneamente, sensores monitoram a temperatura e a umidade do ambiente. Caso o clima fique muito quente ou frio (fora dos padrões ideais para a saúde e bem-estar do pet), o sistema coleta, envia os dados para a nuvem e os exibe em um Dashboard interativo.

## 🛠️ Tecnologias e Ferramentas Utilizadas
* **ESP32 (Simulado no Wokwi):** Microcontrolador com Wi-Fi nativo responsável por processar as leituras físicas.
* **MFRC522 (Módulo RFID):** Utilizado para a leitura das tags de identificação únicas de cada animal.
* **DHT22 (Sensor de Temperatura e Umidade):** Utilizado para monitoramento do clima no ambiente de internação.
* **Protocolo MQTT & HiveMQ:** Mensageria ágil para comunicação em tempo real entre o hardware e o dashboard.
* **Node-RED:** Plataforma de desenvolvimento visual para a construção do Dashboard e tratamento de dados.

## 📡 Arquitetura de Comunicação e o papel do HiveMQ
O coração da nossa comunicação distribuída é o protocolo **MQTT**, operando através do broker público **HiveMQ** (`broker.hivemq.com`). A escolha dessa arquitetura se dá pela sua baixa latência, leveza na transmissão e perfeito desacoplamento entre hardware e software.

O fluxo de funcionamento ocorre em duas vias principais:
1. **Publicação (Publisher - Wokwi/ESP32):** O ESP32 realiza a leitura dos sensores físicos. Ao se conectar ao Wi-Fi virtual, ele publica mensagens contendo blocos JSON diretamente no HiveMQ em tópicos específicos:
   * `clyvovet/internacao/temp`: Recebe os envios constantes com as medições de temperatura e umidade.
   * `clyvovet/triagem/rfid`: Recebe o ID (UID) da coleira do pet instantaneamente assim que a tag RFID é aproximada do leitor.
2. **Subscrição (Subscriber - Node-RED):** Do outro lado da arquitetura, o nosso servidor Node-RED atua escutando a internet. Através dos nós "MQTT In" inscritos nos tópicos acima no HiveMQ, o Node-RED captura as mensagens em milissegundos, extrai os valores via filtros JSON e atualiza os ponteiros e avisos de texto no nosso Dashboard visual.

## 📁 Estrutura do Repositório
* `codigo_esp32.ino`: Script em C++ com a lógica de inicialização, leitura dos sensores e conexão MQTT.
* `diagram.json`: Arquivo de mapeamento, montagem e ligação do circuito virtual no Wokwi.
* `flow_nodered.json`: Código de exportação de todos os nós e do layout do dashboard no Node-RED.

## 🚀 Instruções de Uso (Como reproduzir)
1. **Simulação do Hardware (Wokwi):**
   * Importe os arquivos `.ino` e `diagram.json` presentes neste repositório para um novo projeto na plataforma [Wokwi](https://wokwi.com/).
   * No "Library Manager" do Wokwi, certifique-se de que as bibliotecas `DHT sensor library for ESPx`, `MFRC522` e `PubSubClient` estão instaladas (Também no arquivo `libraries.txt`).
   * Inicie a simulação. O ESP32 se conectará à rede `Wokwi-GUEST` e ao broker HiveMQ.
2. **Dashboard (Node-RED):**
   * Com o Node-RED rodando (local ou em nuvem), acesse o Menu e vá em **Import**.
   * Cole o conteúdo do nosso arquivo `flow_nodered.json` e clique em importar.
   * Clique no botão vermelho **Deploy** no canto superior direito para conectar os nós ao HiveMQ.
   * Acesse a rota `/ui` (ex: `http://localhost:1880/ui`) para visualizar a interface gráfica.
3. **Execução Integrada:**
   * Na tela do Wokwi, clique sobre o sensor DHT22 e altere a temperatura e umidade. 
   * Simule a passagem de um cartão clicando no leitor RFID.
   * Acompanhe no Dashboard a atualização imediata dos gráficos e do nome/ID do pet identificado!

## 📊 Resultados Parciais e Prova de Conceito
A simulação validou com excelência a viabilidade técnica do projeto. A integração IoT provou ser capaz de coletar variáveis físicas e transmiti-las para um ecossistema visual de forma estável. A comunicação MQTT via HiveMQ anulou problemas de latência, demonstrando ser a infraestrutura ideal para escalar a solução para as clínicas da rede CLYVO VET futuramente.
