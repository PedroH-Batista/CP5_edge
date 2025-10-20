#  Projeto: Vinheria Inteligente - Validação da Arquitetura FIWARE IoT

## Integrantes 


| Gabrielle Zollner | 564460 | 
| Pedro Batista | 563220 | 
| João Felipe Bertini | 563478 |


<img src="img\circuito.png" alt="Print do circuito ESP32 com DHT22 e Potenciômetros para Vinheria"/>

**Link para o Código Fonte Final:** [https://github.com/PedroH-Batista/CP5_edge]

---

##  Explicação do Projeto e Arquitetura 

Este projeto consiste na **Prova de Conceito (PoC)** de um sistema de monitoramento para uma adega, utilizando a plataforma **FIWARE** para gerenciamento do contexto e comunicação bidirecional. O objetivo é proteger a qualidade do vinho monitorando remotamente: **Temperatura, Umidade, Luminosidade e Risco de Alagamento**.

### Detalhes da Implementação Técnica

| Componente | Função | Endpoints | Configuração |
| **Plataforma (E1)** | Hospedagem dos serviços FIWARE em containers Docker. | IP Público: **`20.150.210.54`** | Sistema operacional Linux, Docker Compose. |
| **Context Broker** | Gerenciamento central do contexto (estado dos sensores). | Orion (Porta: `1026`) | Status: UP (Ver Print 1) |
| **IoT Agent** | Gateway de comunicação entre o ESP32 e o Orion. | Porta: `4041` / API Key: **`TEF`** | Status: Provisionado (Ver JSON de Provisionamento) |
| **Dispositivo (E2/E3)** | ESP32 (Simulação Wokwi/VS Code). ID Alocado: **`lamp003`** (na faixa 003-020). | Protocolo: **MQTT/UltraLight 2.0** | Sensores: `t`, `h`, `l`, `a`. |

---

##  Validação das Entregas e Provas de Funcionamento (PoC)

Esta seção apresenta os resultados da PoC e os prints que comprovam a funcionalidade e o alinhamento com os requisitos do trabalho.

### 1. Plataforma IoT Cloud FIWARE

**REQUISITO:** Instalação e demonstração da acessibilidade da arquitetura base.

| ARTEFATO | PROVA DE FUNCIONAMENTO |
| :--- | :--- |
| <img src="img\entrega1-1.png" alt=""/> | **Comprovação de Instalação:** O terminal confirma que todos os GEs (Orion, IoT-Agent, Mosquitto) foram inicializados com sucesso na VM Linux. |
|  <img src="img\entrega 2-1.png" alt=""/> | **Comprovação de Acessibilidade:** O Postman confirmando o status `200 OK` na consulta à versão do Orion Context Broker na porta 1026 do IP Público. |




###  2. Publicação de Dados em Tempo Real 

**REQUISITO:** Criação do simulador ESP32 e envio dos 4 parâmetros de sensor para o FIWARE, com visualização na plataforma.

| ARTEFATO | PROVA DE FUNCIONAMENTO |
| :--- | :--- |
| <img src="img\entrega3-2.png" alt=""/> | O corpo da requisição **POST 3** que registrou o `lamp003`, provando o mapeamento dos atributos (`t`=temperatura, `h`=umidade, `l`=luminosidade, `a`=alagamento). |
| <img src="img\entrega1-2.png" alt=""/> | O console do simulador prova a **conexão MQTT** e o **envio contínuo do payload** UL2.0 para o IoT Agent (`/TEF/lamp003/attrs`). |
| <img src="img\entrega2-2.png" alt=""/>| **Comprovação de Integração (40 pts):** O Postman consultando a entidade (`GET /v2/entities/...`) retorna o status `200 OK` e mostra os valores **numéricos em tempo real** dos 4 sensores. |


### **3. Comunicação Bidirecional 

**REQUISITO:** Demonstrar que a plataforma pode enviar um comando (Southbound) para o dispositivo (`lamp003`).

| ARTEFATO | PROVA DE FUNCIONAMENTO |
| :--- | :--- |
| <img src="img\entrega1-3.png" alt=""/>| **Comprovação de Envio:** O Postman envia o comando (`PATCH`) para a entidade `lamp003` (ex: `{"off": {...}}`) e recebe o status de sucesso (`204 No Content`), confirmando o disparo da ação. |
|  <img src="img\entrega2-3.png" alt=""/>| **Comprovação de Reação:** O screenshot do Wokwi/VS Code mostrando o **LED de status aceso/apagado** e o console serial registrando a mensagem de comando recebida e executada. |


