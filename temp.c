/*
 * temp.c
 *
 *  Created on: Jul 12, 2026
 *      Author: Raven
 */
#include "temp.h"
void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_Pin;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void delay_us(uint32_t us)
{
    uint32_t cycles = us * (SystemCoreClock / 1000000);

    uint32_t start = DWT->CYCCNT;

    while ((DWT->CYCCNT - start) < cycles);
}

uint8_t DS18B20_Start (void)
{
	uint8_t Response = 0;
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);   // set the pin as output
	HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin low
	delay_us (480);   // delay according to datasheet
	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);    // set the pin as input
	delay_us (80);    // delay according to datasheet
    if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))) Response = 1;    // if the pin is low i.e the presence pulse is detected
    else
    {Response = 0;}
    delay_us (400); // 480 us delay totally.
    return Response;
}

void DS18B20_Write (uint8_t data)
{
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output

	for (int i=0; i<8; i++)
	{
		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin LOW
			delay_us(6);  // wait for 6 us

			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);  // set as input
			delay_us(60);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin LOW
			delay_us(60);  // wait for 60 us

			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
		}
	}
}
uint8_t DS18B20_Read(void)
{
	uint8_t value=0;
	for (int i=0;i<8;i++)
	{
		Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
		HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the data pin LOW
		delay_us(2);  // wait for 2 us
		Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
		delay_us(13);
		if (HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		delay_us(45);  // wait for 60 us
	}
	return value;
}

float temp_read(void)
{
    uint8_t Temp_byte1, Temp_byte2;
    int16_t TEMP;

    if(!DS18B20_Start())
        return -1000;     // error

    DS18B20_Write(0xCC);
    DS18B20_Write(0xBE);

    Temp_byte1 = DS18B20_Read();
    Temp_byte2 = DS18B20_Read();

    TEMP = (Temp_byte2 << 8) | Temp_byte1;

    // Immediately start the next conversion
    if(DS18B20_Start())
    {
    DS18B20_Write(0xCC);
    DS18B20_Write(0x44);
    }
    return TEMP / 16.0f;
}

