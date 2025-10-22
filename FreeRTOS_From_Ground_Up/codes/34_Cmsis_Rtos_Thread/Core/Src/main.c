/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : This code is using a generic RTOS (CMSIS-RTOS). The four
  * tasks count on the same time (the same priority).
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

/* Module Preprocessor Macros ------------------------------------------------*/
#define STACK_SIZE 512

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

typedef uint32_t TaskProfiler_t;
TaskProfiler_t orangeTaskProfiler, redTaskProfiler, greenTaskProfiler, blueTaskProfiler;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/*Printf function*/
int __io_putchar(int ch);
/*Task function prototypes*/
void vBlueLedControllerTask(void *pvParameters);
void vRedLedControllerTask(void *pvParameters);
void vOrangeLedControllerTask(void *pvParameters);
void vGreenLedControllerTask(void *pvParameters);


int main(void)
{
  /*Reset of all peripherals, Initializes the Flash interface and the Systick.*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /*Initializing Kernel*/
  osKernelInitialize();

  /*Data structures to create the threads*/
  osThreadAttr_t greenThreadAtrribs =
  {
		  .name = "GreenTask",
		  .stack_size = STACK_SIZE,
		  .priority = osPriorityNormal
  };

  osThreadAttr_t redThreadAtrribs =
  {
		  .name = "RedTask",
		  .stack_size = STACK_SIZE,
		  .priority = osPriorityNormal
  };

  osThreadAttr_t orangeThreadAtrribs =
  {
		  .name = "OrangeTask",
		  .stack_size = STACK_SIZE,
		  .priority = osPriorityNormal
  };

  osThreadAttr_t blueThreadAtrribs =
  {
		  .name = "BlueTask",
		  .stack_size = STACK_SIZE,
		  .priority = osPriorityNormal
  };

  /*Create a threads (tasks)*/
  osThreadNew(vGreenLedControllerTask, NULL, &greenThreadAtrribs);
  osThreadNew(vRedLedControllerTask, NULL, &redThreadAtrribs);
  osThreadNew(vOrangeLedControllerTask, NULL, &orangeThreadAtrribs);
  osThreadNew(vBlueLedControllerTask, NULL, &blueThreadAtrribs);

  /*Create a kernel (start the scheduler)*/
  osKernelStart();


  while (1)
  {

  }

}


/**
 * @brief Task function to control a blue led
 */
void vBlueLedControllerTask(void *pvParameters)
{
	while(1)
	{
		blueTaskProfiler++;
		for(int i=0; i<=100000; i++){}
	}
}

/**
 * @brief Task function to control a red led
 */
void vRedLedControllerTask(void *pvParameters)
{
	while(1)
	{
		redTaskProfiler++;
		for(int i=0; i<=100000; i++){}
	}
}

/**
 * @brief Task function to control a green led
 */
void vGreenLedControllerTask(void *pvParameters)
{
	while(1)
	{
		greenTaskProfiler++;
		for(int i=0; i<=100000; i++){}
	}
}

/**
 * @brief Task function to control a orange led
 */
void vOrangeLedControllerTask(void *pvParameters)
{
	while(1)
	{
		orangeTaskProfiler++;
		for(int i=0; i<=100000; i++){}
	}
}

/**
 * @brief UART transmit configuration to use printf function
 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  /*USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

