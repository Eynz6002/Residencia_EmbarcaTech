# Apresentação

Este projeto demonstra uma aplicação simples de Internet das Coisas (IoT) utilizando o microcontrolador Raspberry Pi Pico W. Eles leem o estado da placa e expõe essas informações via um servidor web TCP/IP, acessível em uma rede local via Wi-Fi.

## Pré-requisitos

Para compilar e rodar este código, você precisará configurar o ambiente de desenvolvimento para o Raspberry Pi Pico SDK.

1.  **VS Code**

2.  **Extensão (Raspberry Pi Pico)**

3.  **Placa BitDogLab**

## Configuração do Código

O código possui uma configuração essencial que você **precisa alterar** antes de compilar: as credenciais da sua rede Wi-Fi.

Abra o arquivo `.c` ([U2_C3_Question1.c](U2_C3_Question1) ou [U2_C3_Question2.c](U2_C3_Question2)) em um editor de texto. Procure pelas seguintes linhas:

```c
#define WIFI_SSID "<Nome_da_rede>"
#define WIFI_PASSWORD "<Senha_da_rede>"
```

Altere ambas para o nome da sua internet e a senha da mesma. Salve e compile o projeto que deseja.