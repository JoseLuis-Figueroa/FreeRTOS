/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : The microcontroller use the ISR to handle the USART and
  * receive a string of the computer. The code does not work, the problem is
  * related to the baud rate. It was not fix as it is using the stm32 hal.
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

/* Module Preprocessor Macros ------------------------------------------------*/
#define STACK_SIZE 128	/*128*4 = 512*/
#define EXPECTED_PACKAGE_LENGTH 5

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/*Tasks function prototype*/
void handlerTask(void *pvParameters);
int32_t startRxInterrupt(uint8_t * Buffer, uint_fast16_t Length);

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
static QueueHandle_t UART2_BytesReceived = NULL;
static SemaphoreHandle_t rxDone = NULL;

volatile int rxInProgress = 0;
static uint16_t rxLength = 0;
static uint8_t * rxBuffer = NULL;
static uint16_t rxIteration = 0;


int main(void)
{
  /*Reset of all peripherals, Initializes the Flash interface and the Systick.*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /*Create semaphore*/
  rxDone = xSemaphoreCreateBinary();

  /*Tasks creation*/
  /*tskIDLE_PRIORITY + 3 is three times above of the lowest priority*/
  xTaskCreate(handlerTask, "uartPrintTask", STACK_SIZE, NULL,
		  tskIDLE_PRIORITY + 3, NULL);

  /*Queue creation*/
  UART2_BytesReceived = xQueueCreate(10, sizeof(char));

  /*Start the scheduler*/
  vTaskStartScheduler();


  while (1)
  {

  }

}


/**
  * @brief Sets up an interrupt rx for USART2
  */
int32_t startRxInterrupt(uint8_t * buffer, uint_fast16_t length)
{
	if(!rxInProgress && (rxBuffer != NULL))
	{
		rxInProgress = 1;
		rxLength = length;
		rxBuffer = buffer;
		rxIteration = 0;
		/*Enable RX interrupt*/
		USART2->CR1 |= USART_CR1_RXNEIE;
		NVIC_SetPriority(USART2_IRQn, 6);
		NVIC_EnableIRQ(USART2_IRQn);
		return 0;
	}
	return -1;
}

/**
  * @brief Handler Task function
  */
uint8_t rxData[EXPECTED_PACKAGE_LENGTH];
char rxCode[50] = {0};

void handlerTask(void *pvParameters)
{
	const TickType_t timeout = pdMS_TO_TICKS(8000);

	/*Initialization rxData*/
	for(int i=0; i<EXPECTED_PACKAGE_LENGTH; i++)
	{
		rxData[i] = 0;
	}

	USART2_UART_RX_Init();
	startRxInterrupt(rxData, EXPECTED_PACKAGE_LENGTH);

	while(1)
	{
		startRxInterrupt(rxData, EXPECTED_PACKAGE_LENGTH);

		if(xSemaphoreTake(rxDone, timeout) == pdPASS)
		{
			if(EXPECTED_PACKAGE_LENGTH == rxIteration)
			{
				sprintf(rxCode, "Received");
			}
			else
			{
				sprintf(rxCode, "Length mismatch");
			}
		}
		else
		{
			sprintf(rxCode, "Timeout");
		}

	}
}

/**
  * @brief USART2 interruption routine
  */
void USART2_IRQHandler(void)
{
	/*This indicates when the interrupt finishes to continue with the task that
	 * was left before starting the interrupt. Don't wake up the high priority
	 * task*/
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if(USART2->SR & USART_SR_RXNE)
	{
		uint8_t temperatureValue = (uint8_t) USART2->DR;

		if(rxInProgress)
		{
			rxBuffer[rxIteration++] = temperatureValue;
			if(rxLength == rxIteration)
			{
				rxInProgress = 0;
				xSemaphoreGiveFromISR(rxDone, &xHigherPriorityTaskWoken);
			}
		}
	}
	/*Yield the processor to the scheduler*/
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}



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



