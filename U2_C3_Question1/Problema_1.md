# **Primeira Questão - Monitoramento de Botões e Sensor de Temperatura**

## **Hardware Utilizado**

* **Módulo de Conexão Sem Fio (Infineon CYW43439)**
* **Botões A (GPIO 5)**
* **Botões B (GPIO 6)**
* **Sensor de Temperatura (ADC 4)**

## **Funcionamento**

O programa inicia um servidor web para exibir se os botões A ou B está sendo pressionado. Além disso, também é feito o monitoramento da temperatura interna da placa.

## **Descrição do Funcionamento**

### **Inicialização dos GPIOs e do ADC**

* **Inicialização dos Botões**
```c
stdio_init_all();
    //Inicialização de GPIOs e ADC
    init_buttons();
    init_temp_sensor();
```

* **Funcões**

* **[Aqui](source/adc_controller.c)**
```c
void init_temp_sensor()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
}
```

* **[Aqui](source/buttons_leds.c)**
```c
void init_buttons()
{
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);
    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);
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

### **Monitora se algum dos botões está pressionado**

```c
        //Lógica de estados do botão
        if(!gpio_get(button_A)){
            snprintf(status_button, sizeof(status_button), " A pressionado");
        }
        else if(!gpio_get(button_B)){
            snprintf(status_button, sizeof(status_button), " B pressionado");
        } else {
            snprintf(status_button, sizeof(status_button), " Pressione A ou B");
        }
```

### **Monitora a temperatura**

* **Main.c**
```c
        //Monitoramento de temperatura interna
        temperature = temperature_check();
```

* **Função ([adc_controller](source/adc_controller.c))**
```c
float temperature_check()
{
    adc_select_input(adc_temp_sensor);
    uint16_t raw_value = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    return  27.0f - ((raw_value * conversion_factor) - 0.706f) / 0.001721f;
    
}
```

### **Espera 1000 milissegundos**

```c
        //Redução de consumo de processamento
        sleep_ms(1000);
```

### **Repete o Processo**