/*
 * exti.h
 *
 *  Created on: Jan 29, 2024
 *      Author: Jose Figueroa
 */

#ifndef EXTI_H_
#define EXTI_H_

void INTI_p13Init(void);
void INTI_GPIO_Init(void);
uint8_t INTI_GPIO_readDigitalSensor(void);


#endif /* EXTI_H_ */
