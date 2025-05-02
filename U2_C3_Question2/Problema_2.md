# **Segunda Questão - Monitoramento de Joystick**

## **Hardware Utilizado**

* **Módulo de Conexão Sem Fio (Infineon CYW43439)**
* **Joystick (ADC 26 e 27)**

## **Funcionamento**

O programa inicia um servidor web para exibir a direção do joystick (a exibição do joystick é no formato de uma rosa dos ventos).

## **Descrição do Funcionamento**

### **Inicialização do ADC**

```c
stdio_init_all();
    //Inicialização do ADC
    init_joystick();
```

* **[Função de inicialização](source/adc_controller.c)**

```c
void init_joystick()
{
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
}
```

### **Inicialização da Conexão com a Rede Local**

```c
//Inicialização do wifi, se não conectar retorna -1
    while (cyw43_arch_init())
    {
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    //Inicializa a placa como cliente
    cyw43_arch_enable_sta_mode();

    //Tenta conectar ao Wifi, se não retorna -1
    printf("Conectando ao Wi-Fi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000))
    {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }
```

### **Exibe o IP para conexão com a Placa**

```c
//Exibe o IP da placa
    if (netif_default)
    {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }
```

### **Configura o Servidor**

```c
// Configura o servidor TCP
    struct tcp_pcb *server = tcp_new();//Cria novo pcb para o servidor tcp
    
    if (!server)//Verifica se criação falhou
    {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK)//Associa o pcb do servidor local ao IP e à porta 80
    {
        printf("Falha ao associar servidor TCP à porta 80\n");
        return -1;
    }

    server = tcp_listen(server);//Configura o servidor para 'listening'
    
    //Configura 'tcp_server_accept' como callback para novas tentativas de conexão ao servidor
    tcp_accept(server, tcp_server_accept);

    printf("Servidor ouvindo na porta 80\n");
```

### **Loop Inicia**

```c
while (true)
{
    ...
}
```

### **Processa os eventos pendentes**

```c
        cyw43_arch_poll();//permite processamento de eventos pendentes
```

### **Leitura do joystick**

```c
        //Lê os valores atuais do joystick.
        read_joystick();
```

* **[Função de leitura do joystick](source/adc_controller.c)**

```c
void read_joystick()
{
    adc_select_input(X_joystick);
    x_location = adc_read();
    adc_select_input(Y_joystick);
    y_location = adc_read();
}
```

### **Monitoramento das direções**

```c
        //Monitoramento do joystick
        rosa_dos_ventos();
```

* **Função de monitoramento**

```c
//Monitoramento do joystick e configuração da rosa dos ventos
void rosa_dos_ventos()
{
    // Verifica se o joystick está significativamente para CIMA (valor Y maior que o centro + zona morta)
    if(y_location > (center_y + dead_zone))
    {
        if(x_location > (center_x + dead_zone)) {
            sprintf(direction, "Nordeste"); 
        }
        else if(x_location < (center_x - dead_zone)) {
            sprintf(direction, "Noroeste"); 
        }
        else { sprintf(direction, "Norte"); }
    }
    // Senão, verifica se o joystick está significativamente para BAIXO (valor Y menor que o centro - zona morta)
    else if(y_location < (center_y - dead_zone))
    {
        if(x_location > (center_x + dead_zone)) { 
            sprintf(direction, "Sudeste"); 
        }
        else if(x_location < (center_x - dead_zone)) { 
            sprintf(direction, "Sudoeste"); 
        }
        else { 
            sprintf(direction, "Sul"); 
        }
    }
    // Senão (se Y está dentro da zona morta vertical), verifica apenas a posição X:
    else if(x_location > (center_x + dead_zone)) { 
        sprintf(direction, "Leste"); 
    }
    // - Significativamente para ESQUERDA -> OESTE
    else if(x_location < (center_x - dead_zone)) { 
        sprintf(direction, "Oeste"); 
    }
    // Se está dentro da zona morta tanto de X quanto de Y -> CENTRO
    else { 
        sprintf(direction, "Centro"); 
    }

}
```

### **Espera 1000 milissegundos**

```c
        //Redução de consumo de processamento
        sleep_ms(1000);
```

### **Repete o Processo**