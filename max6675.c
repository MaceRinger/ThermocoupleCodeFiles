#include "max6675.h"

static inline void cs_low(const max6675_t *d) {
    HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_RESET);
}

static inline void cs_high(const max6675_t *d) {
    HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_SET);
}

HAL_StatusTypeDef max6675_read_c(const max6675_t *dev, float *temp_c, uint16_t *raw)
{
    uint8_t tx[2] = {0x00, 0x00};
    uint8_t rx[2] = {0x00, 0x00};

    cs_low(dev);
    for (volatile int i = 0; i < 200; i++) __NOP();  // small CS->SCK delay

    if (HAL_SPI_TransmitReceive(dev->hspi, tx, rx, 2, 50) != HAL_OK) {
        cs_high(dev);
        return HAL_ERROR;
    }

    cs_high(dev);

    uint16_t v = ((uint16_t)rx[0] << 8) | rx[1];
    if (raw) *raw = v;

    if (v & 0x0004) {                 // D2 = thermocouple open
        return HAL_ERROR;
    }

    uint16_t t12 = (v >> 3) & 0x0FFF; // 12-bit temp
    if (temp_c) *temp_c = 0.25f * (float)t12;

    return HAL_OK;
}
