/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Use event groups to run 3 tasks of the same level in
  * order. Use another task to print the results of all of them.
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
#define TASK1_BIT (1UL<<0UL)
#define TASK2_BIT (1UL<<1UL)
#define TASK3_BIT (1UL<<2UL)

const TickType_t xDelay500ms = pdMS_TO_TICKS(500UL);

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/*Task function prototypes*/
static void inputTask1(void *pvParameters);
static void inputTask2(void *pvParameters);
static void inputTask3(void *pvParameters);

static void outputTask(void *pvParameters);

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
/*Event declaration*/
EventGroupHandle_t xEventGroup;


int main(void)
{
	/*Reset of all peripherals, Initializes the Flash interface and the Systick.*/
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	/*Initialize the TX transmitter*/
	USART2_UART_TX_Init();

	printf("System Initializing ... \r\n");

	/*Group creation*/
	xEventGroup = xEventGroupCreate();

	/*Tasks Creation*/
	xTaskCreate(inputTask1, "InputTask 1", 100, NULL, 1, NULL);
	xTaskCreate(inputTask2, "InputTask 2", 100, NULL, 1, NULL);
	xTaskCreate(inputTask3, "InputTask 3", 100, NULL, 1, NULL);

	xTaskCreate(outputTask, "OutputTask", 100, NULL, 1, NULL);

	/*Task scheduler*/
	vTaskStartScheduler();

	while (1)
	{

	}

}


/**
  * @brief input task 1 function implementation
  */
static void inputTask1(void *pvParameters)
{
	while(1)
	{
		/*Set the event to indicate the this task is done, and it can follow
		 * the next task*/
		xEventGroupSetBits(xEventGroup, TASK1_BIT);
	}
}

/**
  * @brief input task 2 function implementation
  */
static void inputTask2(void *pvParameters)
{
	while(1)
	{
		/*Set the event to indicate the this task is done, and it can follow
		 * the next task*/
		xEventGroupSetBits(xEventGroup, TASK2_BIT);
	}
}

/**
  * @brief input task 3 function implementation
  */
static void inputTask3(void *pvParameters)
{
	while(1)
	{
		/*Set the event to indicate the this task is done, and it can follow
		 * the next task*/
		xEventGroupSetBits(xEventGroup, TASK3_BIT);
	}
}

/**
  * @brief output task function implementation
  */
static void outputTask(void *pvParameters)
{
	/*Merge the tasks bits into one variable*/
	const EventBits_t xBitsToWaitFor = TASK1_BIT | TASK2_BIT | TASK3_BIT;

	EventBits_t xEventGroupValue;

	while(1)
	{
		/*Gets the event group value to read*/
		xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE, pdFALSE, portMAX_DELAY);

		/*Implement the task according to the task bit of the event group value*/
		if ((xEventGroupValue & TASK1_BIT) != 0)
		{
			printf("Bit reading task -\t event bit 0 was set - TASK1\r\n");
		}
		if ((xEventGroupValue & TASK2_BIT) != 0)
		{
			printf("Bit reading task -\t event bit 1 was set - TASK2\r\n");
		}
		if ((xEventGroupValue & TASK3_BIT) != 0)
		{
			printf("Bit reading task -\t event bit 2 was set - TASK3\r\n");
		}
	}
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



