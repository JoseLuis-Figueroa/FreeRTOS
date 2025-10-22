/*
 * exti.c
 *
 *  Created on: Jan 29, 2024
 *      Author: Jose Figueroa
 */
#include "stm32f4xx_hal.h"

void INTI_p13Init(void)
{
	/*Enable GPIOC clock*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	/*Configure p13 as input*/
	GPIOC->MODER &=~ GPIO_MODER_MODER13_0;
	GPIOC->MODER &=~ GPIO_MODER_MODER13_0;

	/*Enable SYSCFG clock*/
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	/*Clear port selection for EXTI13*/
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
	/*Unmask EXTI13*/
	EXTI->IMR |= EXTI_IMR_MR13;
	/*Select falling edge trigger*/
	EXTI->FTSR |= EXTI_FTSR_TR13;
	/*Set priority to the external interrupt*/
	NVIC_SetPriority(EXTI15_10_IRQn, 6);
    /*Enable EXTI13 line in NVIC(ARM Cortex)*/
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    /*Enable global interrupts*/
    __enable_irq();
}

void INTI_GPIO_Init(void)
{
	/*Enable GPIOC clock*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	/*Configure p13 as input*/
	GPIOC->MODER &=~ GPIO_MODER_MODER13_0;
	GPIOC->MODER &=~ GPIO_MODER_MODER13_0;

}

uint8_t INTI_GPIO_readDigitalSensor(void)
{
	if (GPIOC->IDR & 0x2000)
	{
		return 0x1;
	}
	else
	{
		return 0x0;
	}
}
