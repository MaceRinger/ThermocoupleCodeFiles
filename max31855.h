#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
} max31855_t;

typedef struct {
    float tc_c;     // thermocouple temp
    float int_c;    // internal junction temp
    uint8_t fault;  // 1 if any fault
    uint8_t oc;     // open circuit
    uint8_t scg;    // short to GND
    uint8_t scv;    // short to VCC
    uint32_t raw;
} max31855_reading_t;

HAL_StatusTypeDef max31855_read(const max31855_t *dev, max31855_reading_t *out);
