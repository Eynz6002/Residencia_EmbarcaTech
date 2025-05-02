#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "libs/buttons_leds.h"
#include "libs/adc_controller.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

#define WIFI_SSID "<Nome_da_rede>"
#define WIFI_PASSWORD "<Senha_da_rede>"

char html[1024];
char status_button[20];
float temperature;

//Configuração da página html
void html_formator()
{
    snprintf(html, sizeof(html),
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html;"
    "charset=UTF-8"
    "\r\n"
    "\r\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<meta charset=\"UTF-8\">"
    "<head>\n"
        "<title>Button Monitor</title>\n"
    "</head>\n"
    "<body style='margin: 50px;' bgcolor='#000000'>"
        "<table align='center' border='1' bordercolor='#169976'>"
            "<tr>"
                "<td colspan='2' width='800px'>"
                    "<p align='center'>"
                        "<font color='#169976' face='arial' size='8'>"
                            "Monitoramento de Botão com Sensor"
                        "</font>"
                    "</p>"
                "</td>"
            "</tr>"
            "<tr>"
                "<td width='200px'>"
                    "<font face='arial' size='5' color='#169976'>"
                        "Botão"
                    "</font>"
                "</td>"
                "<td width='600px' align='center'>"
                    "<font face='arial' size='5' color='#169976'>"
                        "%s"
                    "</font>"
                "</td>"
            "</tr>"
            "<tr>"
                "<td width='200px'>"
                    "<font face='arial' size='5' color='#169976'>"
                        "Temperatura:"
                    "</font>"
                "</td>"
                "<td width='600px' align='center'>"
                    "<font face='arial' size='5' color='#169976'>"
                        "%.2f"" &deg;C"
                    "</font>"
                "</td>"
            "</tr>"
        "</table>"
        "<script>\n"
            // Script JavaScript para fazer a página recarregar automaticamente
            "setTimeout(() => { window.location.href = \"/\"; }, 1000);\n"
        "</script>\n"
    "</body>\n"
    "</html>\r\n", status_button, temperature);
}
//Chama a pilha de rede quando pacotes são recebidos
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)//Verifica se o cliente está conectado
    {
        tcp_close(tpcb);//Fecha a sessão se for verdadeiro
        return ERR_OK;//Retorna sucesso
    }

    html_formator();//Chama a formatação do html -> garante que os dados sejam os atualizados

    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);//Envia o html atualizado para o cliente por tcp
    tcp_output(tpcb);//Realiza saída tcp para o cliente

    pbuf_free(p);//Limpa os dados de pacotes recebidos

    return ERR_OK;//Retorna sucesso
}
//Callback para nova conexão do cliente
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_server_recv);//Chama outra função para recepção de dados
    return ERR_OK;//Retorna sucesso de conexão
}
int main()
{
    stdio_init_all();
    //Inicialização de GPIOs e ADC
    init_buttons();
    init_temp_sensor();

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
    
    printf("Conectado ao Wi-Fi\n");
    
    //Exibe o IP da placa
    if (netif_default)
    {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

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

    while (true) {
        cyw43_arch_poll();//permite processamento de eventos pendentes

        //Lógica de estados do botão
        if(!gpio_get(button_A)){
            snprintf(status_button, sizeof(status_button), " A pressionado");
        }
        else if(!gpio_get(button_B)){
            snprintf(status_button, sizeof(status_button), " B pressionado");
        } else {
            snprintf(status_button, sizeof(status_button), " Pressione A ou B");
        }

        //Monitoramento de temperatura interna
        temperature = temperature_check();

        //Redução de consumo de processamento
        sleep_ms(1000);
    }
}