#ifndef AHT10_H
#define AHT10_H

#include "hardware/i2c.h"
#include "stdint.h"

typedef struct {
    uint SDA_PIN;
    uint SCL_PIN;
    i2c_inst_t* I2C_PORT;
} aht10;

#define AHT10_ADDR 0x38

void aht10_init(uint SDA_PIN, uint SCL_PIN, i2c_inst_t* I2C_PORT);
void aht10_trigger_monitoring();
void aht10_read(float data_output[]);

#endif