#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "libs/adc_controller.h"

#define WIFI_SSID "MillyPhb_ENZO"
#define WIFI_PASSWORD "ejel1711"

char html[1024]; //Armazena o código HTML da página web a ser enviada
char direction[15]; //Armazena a direção calculada.


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
        "<title>Rosa dos Ventos</title>\n"
    "</head>\n"
    "<body align='center' bgcolor='#000000'>\n"
        "<table align='center' border='1' bordercolor='#169976'>"
            "<tr>"
                "<td>"
                    "<p align='center'>"
                        "<font color='#169976' face='arial' size='8'>"
                            "Rosa dos Ventos"
                        "</font>"
                    "</p>"
                "</td>"
            "</tr>"
            "<tr>"
                "<td>"
                    "<table align='center' border='1' bordercolor='#169976'>"
                        "<tr>"
                            "<td width='100px'>"
                                "<font face='arial' size='5' color='#169976'>"
                                    "Direção"
                                "</font>"
                            "</td>"
                            "<td width='100px' align='center'>"
                                "<font face='arial' size='5' color='#169976'>"
                                    "%s"
                                "</font>"
                            "</td>"
                        "</tr>"
                    "</table>"
                "</td>"
            "</tr>"
        "</table>"
    // Script JavaScript para fazer a página recarregar automaticamente
    "<script>\n"
        "setTimeout(() => { window.location.href = \"/\"; }, 1000);\n"
    "</script>\n"
    "</body>\n"
    "</html>\r\n",
    direction);    
}

//Chama a pilha de rede quando pacotes são recebidos
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    //Verifica se o cliente encerrou a sessão
    if (!p)
    {
        tcp_close(tpcb);//Fecha a conexão TCP do lado do servidor
        return ERR_OK;//Retorna sucesso
    }

    //Formata o HTML com a direção atualizada do joystick
    html_formator();

    //Envia o HTML formatado para o cliente pelo TCP.
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);

    //Realiza saída tcp para o cliente
    tcp_output(tpcb);
    
    //Limpa os dados de pacotes recebidos
    pbuf_free(p);

    //Retorna sucesso
    return ERR_OK;
}
//Callback para nova conexão do cliente
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_server_recv);//Chama outra função para recepção de dados
    return ERR_OK;//Retorna sucesso.
}
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
int main()
{
    stdio_init_all();
    //Inicialização do ADC
    init_joystick();

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
    
    //Exibe o IP da placa
    printf("Conectado ao Wi-Fi\n");
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

    printf("Iniciando loop principal...\n");
    while (true) {
        //Mantem a conexão Wi-Fi ativa. DEVE ser chamado regularmente.
        cyw43_arch_poll();
        //Lê os valores atuais do joystick.
        read_joystick();
        //Monitoramento do joystick
        rosa_dos_ventos();
        //Pausa para evitar excesso de uso da CPU
        sleep_ms(100);
    }

    // O código teoricamente nunca chega aqui devido ao 'while(true)'
    return 0;
}