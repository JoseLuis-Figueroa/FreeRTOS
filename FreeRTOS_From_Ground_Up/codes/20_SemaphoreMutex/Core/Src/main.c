/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : This code uses a mutex to handle two tasks (gpio and
  * adc drivers).
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"
#include "uart.h"
#include "exti.h"
#include "adc.h"


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/*Task function prototypes*/
void analogSensorTask(void *pvParameters);
void digitalSensorTask(void *pvParameters);

/* Private variables ---------------------------------------------------------*/
uint8_t buttonState;
uint32_t sensorValue;

SemaphoreHandle_t xSerialSemaphore;


int main(void)
{
  /*Reset of all peripherals, Initializes the Flash interface and the Systick.*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  USART2_UART_TX_Init();

  /*To make sure the initializing peripherals is done*/
  printf("System Initializing...\n\r");

  /*Create a mutex semaphore*/
  xSerialSemaphore = xSemaphoreCreateMutex();

  /*Tasks creation list*/
  xTaskCreate(digitalSensorTask, "ButtonRead", 128, NULL, 1, NULL);
  xTaskCreate(analogSensorTask, "SensorRead", 128, NULL, 1, NULL);

  /*Start scheduler*/
  vTaskStartScheduler();


  while (1)
  {

  }

}

/* Task Implementation ------------------------------------------------------*/

/**
 * @brief Digital Sensor Task
 */
void digitalSensorTask(void *pvParameters)
{
	/*Initialize the GPIO as input*/
	INTI_GPIO_Init();
	while(1)
	{
		/*Review if the board button (p13) is pressed*/
		buttonState = INTI_GPIO_readDigitalSensor();
		/*Review if we can obtain or "Take" the serial semaphore. If the
		 * semaphore is not available, wait 5 ticks of the scheduler to see
		 * if it becomes free.*/
		if(xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
		{
			printf("The button state is: %d \r\n", buttonState);
			/*Free or "give" the serial port to others tasks*/
			xSemaphoreGive(xSerialSemaphore);
		}
		vTaskDelay(1);
	}
}


/**
 * @brief Analog Sensor Task
 */
void analogSensorTask(void *pvParameters)
{
	/*Initialize the ADC*/
	ADC_init();
	while(1)
	{
		/*Reading the ADC*/
		sensorValue = ADC_readAnalogSensor();
		/*Review if we can obtain or "Take" the serial semaphore. If the
		 * semaphore is not available, wait 5 ticks of the scheduler to see
		 * if it becomes free.*/
		if(xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
		{
			printf("The sensor value is: %ld \r\n", sensorValue);
			/*Free or "give" the serial port to others tasks*/
			xSemaphoreGive(xSerialSemaphore);
		}
		vTaskDelay(1);
	}
}


/* Private function definition-----------------------------------------------*/
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}



