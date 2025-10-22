/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Create a Queueset. It contains two queues that are used on
  * two different tasks that are sent to a receiver task to print it.
  * Note: I increased the stack memory on xTaskCreate function so that the code
  * works correctly.
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

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/*Printf function*/
int __io_putchar(int ch);

/*Task function prototypes*/
void vSenderTask1(void *pvParameters);
void vSenderTask2(void *pvParameters);
void vReceiverTask(void *pvParameters);

/* Queue creation ------------------------------------------------------------*/
static QueueHandle_t xQueue1= NULL, xQueue2 = NULL;

/*Declare a queue set*/
static QueueSetHandle_t xQueueSet = NULL;



int main(void)
{
  /*Reset of all peripherals, Initializes the Flash interface and the Systick.*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  printf("System initializing...\r\n");

  /*Create two queues, each sends a character pointer*/
  xQueue1 = xQueueCreate(1, sizeof(char *));
  xQueue2 = xQueueCreate(1, sizeof(char *));

  /*Create a queue set to hold 2 queue each queue holds 1 element*/
  xQueueSet = xQueueCreateSet(1 * 2);

  /*Add the two queues to the queue set*/
  xQueueAddToSet(xQueue1, xQueueSet);
  xQueueAddToSet(xQueue2, xQueueSet);

  /*Tasks creation*/

  /*Create two sender tasks, with the same priority of 1*/
  xTaskCreate(vSenderTask1, "Sender1", 1000, NULL, 1, NULL);
  xTaskCreate(vSenderTask1, "Sender2", 1000, NULL, 1, NULL);

  /*Create the receiver task, with a higher priority of 2*/
  xTaskCreate(vReceiverTask, "Receiver", 1000, NULL, 2, NULL);

  /*Start scheduler*/
  vTaskStartScheduler();


  while (1)
  {

  }

}


/**
 * @brief Sender 1 task
 */
void vSenderTask1(void *pvParameters)
{
	/*Convert the ticks to 100ms*/
	const TickType_t xBlockTime = pdMS_TO_TICKS(100);
	char * message = "Message from vSenderTask1\r\n";

	while(1)
	{
		/*Block for 100ms*/
		vTaskDelay(xBlockTime);

		/*Send the string "message" to xQueue1*/
		xQueueSend(xQueue1, &message, 0);
	}

}

/**
 * @brief Sender 2 task
 */
void vSenderTask2(void *pvParameters)
{
	/*Convert the ticks to 200ms*/
	const TickType_t xBlockTime = pdMS_TO_TICKS(200);
	char * message = "Message from vSenderTask2\r\n";

	while(1)
	{
		/*Block for 200ms*/
		vTaskDelay(xBlockTime);

		/*Send the string "message" to xQueue2*/
		xQueueSend(xQueue2, &message, 0);
	}

}

/**
 * @brief Receiver task
 */
void vReceiverTask(void *pvParameters)
{
	QueueHandle_t xQueueThatContainsData;
	char *pcReceivedString;

	while(1)
	{
		/*Return the queue set that has new data*/
		xQueueThatContainsData = (QueueHandle_t) xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
		/*Receive data from the queue whose handle was returned*/
		xQueueReceive(xQueueThatContainsData, &pcReceivedString, 0);
		/*Print the data*/
		printf("%s",pcReceivedString);
	}
}

/**
 * @brief UART write function
 */
int uart2_write(int ch)
{
	/*Wait until data is transferred(TXE=1). While condition is running while
	 TXE bit is 0 (false(!)) when TXE bit is 1 the while condition ends*/
	while(!(USART2->SR & USART_SR_TXE)){}
	USART2->DR = (ch & 0xFF);

	return ch;
}

/**
 * @brief UART transmit configuration to use printf function
 */
int __io_putchar(int ch)
{
	uart2_write(ch);
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

