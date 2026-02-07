#include "max31855.h"

static inline void cs_low(const max31855_t *d){
    HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_RESET);
}
static inline void cs_high(const max31855_t *d){
    HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_SET);
}

static int32_t sign_extend(int32_t v, int bits)
{
    int32_t m = 1 << (bits - 1);
    return (v ^ m) - m;
}

HAL_StatusTypeDef max31855_read(const max31855_t *dev, max31855_reading_t *out)
{
    uint8_t tx[4] = {0,0,0,0};
    uint8_t rx[4] = {0,0,0,0};

    cs_low(dev);
    for (volatile int i=0; i<200; i++) __NOP();

    if (HAL_SPI_TransmitReceive(dev->hspi, tx, rx, 4, 50) != HAL_OK) {
        cs_high(dev);
        return HAL_ERROR;
    }
    cs_high(dev);

    uint32_t raw = ((uint32_t)rx[0] << 24) |
                   ((uint32_t)rx[1] << 16) |
                   ((uint32_t)rx[2] <<  8) |
                   ((uint32_t)rx[3] <<  0);

    if (!out) return HAL_OK;
    out->raw = raw;

    out->fault = (raw >> 16) & 1;
    out->oc    = (raw >> 0) & 1;
    out->scg   = (raw >> 1) & 1;
    out->scv   = (raw >> 2) & 1;

    // Thermocouple temp: bits 31..18 (14-bit signed), 0.25 C/LSB
    int32_t tc14 = (raw >> 18) & 0x3FFF;
    tc14 = sign_extend(tc14, 14);
    out->tc_c = (float)tc14 * 0.25f;

    // Internal temp: bits 15..4 (12-bit signed), 0.0625 C/LSB
    int32_t int12 = (raw >> 4) & 0x0FFF;
    int12 = sign_extend(int12, 12);
    out->int_c = (float)int12 * 0.0625f;

    return HAL_OK;
}
