/*
 * adc.c
 *
 *  Created on: Jan 25, 2024
 *      Author: Jose Figueroa
 */

#include "stm32f4xx_hal.h"


void ADC_init(void)
{
	/* Enable clock access to GPIOA and ADC1*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	/*Configure GPIOA1(PA1) as analog mode*/
	GPIOA->MODER |= GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1;

	/*Software trigger*/
	//ADC1->CR2 |= 0x00;
	/*Conversion sequence start. Set the ADC_CH1 (PA1) to start*/
	ADC1->SQR3 |= ADC_SQR3_SQ1_0;
	/*Conversion sequence length. Set the number of conversions in
	 * the regular channel conversion sequence*/
	ADC1->SQR1 |= ADC_SQR3_SQ1_0;
	ADC1->SQR1 |= ADC_SQR3_SQ1_1;
	ADC1->SQR1 |= ADC_SQR3_SQ1_2;
	ADC1->SQR1 |= ADC_SQR3_SQ1_3;
	ADC1->SQR1 |= ADC_SQR3_SQ1_4;
	/*Enable ADC mode*/
	ADC1->CR2 |= ADC_CR2_ADON;
}

uint32_t ADC_readAnalogSensor(void)
{
	/*Start ADC conversion*/
	ADC1->CR2 |= ADC_CR2_SWSTART;
	/*Wait for conversion to be completed*/
	while(!(ADC1->SR & ADC_SR_EOC)){}

	return ADC1->DR;
}
