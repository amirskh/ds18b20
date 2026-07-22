#ifndef INC_TEMP_H_
#define INC_TEMP_H_
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_PIN_4

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DS18B20_Read (void);
void DS18B20_Write (uint8_t data);
uint8_t DS18B20_Start (void);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void delay_us(uint32_t us);

float temp_read(void);
#endif /* INC_TEMP_H_ */
