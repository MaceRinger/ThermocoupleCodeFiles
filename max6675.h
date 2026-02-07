#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
} max6675_t;

HAL_StatusTypeDef max6675_read_c(const max6675_t *dev, float *temp_c, uint16_t *raw);
