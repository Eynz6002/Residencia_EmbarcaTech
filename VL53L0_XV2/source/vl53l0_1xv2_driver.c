#include "pico/stdlib.h"
#include "libs/vl53l0_1xv2.h"
#include <stdlib.h>
#include <stdio.h>

vl53l0_1xv2 sensor;

int config_vl53l0_1xv2(uint SDA_PIN, uint SCL_PIN, i2c_inst_t* i2c_port) {
    sensor.SDA_PIN = SDA_PIN;
    sensor.SCL_PIN = SCL_PIN;
    sensor.I2C_PORT = i2c_port;

    i2c_init(sensor.I2C_PORT, 100 * 1000); // Comunicação I2C a 100 kHz
    gpio_set_function(sensor.SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(sensor.SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(sensor.SDA_PIN);
    gpio_pull_up(sensor.SCL_PIN);

    return 0;
}

int init_vl53l0_1xv2() {
    uint8_t reg = REG_IDENTIFICATION_MODEL_ID;
    uint8_t id; // Variável para armazenar o ID lido

    // 1. Envia o endereço do registrador para leitura
    if (i2c_write_blocking(sensor.I2C_PORT, VL53L0X_ADDR, &reg, 1, true) != 1) {
        printf("Init Error: Nao foi possivel escrever o registrador de ID. Ret: %d\n", PICO_ERROR_GENERIC);
        return 0; // Falha na inicialização
    }

    // 2. Lê o ID do sensor
    if (i2c_read_blocking(sensor.I2C_PORT, VL53L0X_ADDR, &id, 1, false) != 1) {
        printf("Init Error: Nao foi possivel ler o ID do sensor. Ret: %d\n", PICO_ERROR_GENERIC);
        return 0; // Falha na inicialização
    }

    // 3. Verifica se o ID lido é o esperado (0xEE para VL53L0X)
    if (id != 0xEE) {
        printf("Init Error: ID invalido: 0x%02X (esperado: 0xEE)\n", id);
        return 0; // ID incorreto
    }

    // Se chegou aqui, a inicialização foi um sucesso
    return 1;
}

int read_distance() {
    uint8_t cmd[2] = {REG_SYSRANGE_START, 0x01};

    if(i2c_write_blocking(sensor.I2C_PORT, VL53L0X_ADDR, cmd, 2, false) != 2) return -1;

    for (int i = 0; i < 100; i++) {
        uint8_t reg = REG_RESULT_RANGE_STATUS;
        uint8_t status;

        if (i2c_write_blocking(sensor.I2C_PORT, VL53L0X_ADDR, &reg, 1, true) != 1) return -1;

        if (i2c_read_blocking(sensor.I2C_PORT, VL53L0X_ADDR, &status, 1, false) != 1) return -1;

        if (status & 0x01) break;
        sleep_ms(5);
    }

    uint8_t reg = REG_RESULT_RANGE_MM;
    uint8_t buffer[2];

    if (i2c_write_blocking(sensor.I2C_PORT, VL53L0X_ADDR, &reg, 1, true) != 1)
        return -1;
    if (i2c_read_blocking(sensor.I2C_PORT, VL53L0X_ADDR, buffer, 2, false) != 2)
        return -1;

    return (buffer[0] << 8) | buffer[1];
}