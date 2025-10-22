/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : There are two sender with the same priority, the value are
  * sent one after another to the receiver. The receiver shows them using the
  * printf function. The value sent is a struct.
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

TaskHandle_t humidityTaskHandle, pressureTaskHandle, receiverHandle;
QueueHandle_t xQueue;

/* Structs and enums ---------------------------------------------------------*/
typedef enum
{
	humiditySensor,
	pressureSensor
}DataSource_t;

/*Define structure type to be passed to the queue*/
typedef struct
{
	uint8_t ucValue;
	DataSource_t sDataSource;
}Data_t;

/*Declare an array of two "Data_t" variables that will be passed to the queue*/
static const Data_t xStructsToSend[2] =
{
		{77, humiditySensor},	/*Used by humidity sensor*/
		{63, pressureSensor}	/*Used by pressure sensor*/
};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/*Printf function*/
int __io_putchar(int ch);

/*Task function prototypes*/
void senderTask(void *pvParmeters);
void receiverTask(void *pvParameters);

/* Queue creation ------------------------------------------------------------*/
QueueHandle_t yearQueue;


int main(void)
{
  /*Reset of all peripherals, Initializes the Flash interface and the Systick.*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /*Create a receiver task with a priority of 1*/
  xTaskCreate(receiverTask, "Receiver Task", 100, NULL, 1, &receiverHandle);
  /*Create task to send humidity data with a priority of 2*/
  xTaskCreate(senderTask, "Humidity Sender Task", 100, (void *)&(xStructsToSend[0]),
		  2, &humidityTaskHandle);
  /*Create task to send pressure data with a priority of 2*/
  xTaskCreate(senderTask, "Pressure Sender Task", 100, (void *)&(xStructsToSend[1]),
		  2, &pressureTaskHandle);

  /*Start the scheduler*/
  vTaskStartScheduler();

  while (1)
  {

  }

}


/**
 * @brief Sender task
 */
void senderTask(void *pvParmeters)
{
	BaseType_t qStatus;
	/* Enter the blocked state for 200ms for space to become available in the queue
	 * each time the queue is full */
	const TickType_t waitTime = pdMS_TO_TICKS(200);

	/*Create queue to hold a maximum of 3 structures*/
	xQueue = xQueueCreate(3, sizeof(Data_t));

	while(1)
	{
		qStatus = xQueueSend(xQueue, pvParmeters, waitTime);
		/*Send a message if the value was not sent*/
		if(qStatus != pdPASS)
		{
			printf("Error: Data was not sent correctly \r\n");
		}
	}
	for(volatile int i=0; i<1000000; i++);
}

/**
 * @brief Receiver task
 */
void receiverTask(void *pvParameters)
{
	Data_t xReceivedStructure;
	BaseType_t qStatus;

	while(1)
	{
		qStatus = xQueueReceive(xQueue, &xReceivedStructure, 0);
		if(qStatus == pdPASS)
		{
			if(xReceivedStructure.sDataSource == humiditySensor)
			{
				printf("Humidity sensor value = %d \r\n", xReceivedStructure.ucValue);
			}
			if(xReceivedStructure.sDataSource == pressureSensor)
			{
				printf("Pressure sensor value = %d \r\n", xReceivedStructure.ucValue);

			}

		}
		else
		{
			printf("Error: Data was not received \r\n");
		}

	}
	for(volatile int i=0; i<1000000; i++);

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

