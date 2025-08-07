#ifndef BH1750_H
#define BH1750_H

#include "hardware/i2c.h"
#include "stdint.h"

// Configurações do Sensor BH1750
#define BH1750_ADDR 0x23    // Endereço I2C do sensor de luz BH1750.
#define BH1750_CMD_START 0x10 // Comando para iniciar medição contínua alta resolução.

typedef struct {
    uint SDA_PIN;
    uint SCL_PIN;
    i2c_inst_t* I2C_PORT;
    uint16_t lecture;
} bh1750;

void config_bh1750(uint SDA_PIN, uint SCL_PIN, i2c_inst_t* I2C_PORT);
void bh1750_start_monitoring();
uint16_t bh1750_read();

#endif