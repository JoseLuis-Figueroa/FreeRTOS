/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Implementation of one shot (Run one time) and auto reload
  *  timers (run continuously). The auto reload timer is stopped after 10 ticks.
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

/*Maximum count*/
#define STOP_MARK 10
/*The periods assigned to the one-shot and auto-reload timers respectively*/
#define mainONE_SHOT_TIMER_PERIOD		(pdMS_TO_TICKS(4000UL))
#define mainAUTO_RELOAD_TIMER_PERIOD	(pdMS_TO_TICKS(500UL))

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void prvOneShotTimerCallback(TimerHandle_t xTimer);
void prvAutoReloadTimerCallback(TimerHandle_t xTimer);

/* Private variables ---------------------------------------------------------*/
uint32_t timeoutCount = 0;

TimerHandle_t xAutoReloadTimer, xOneShotTimer;
BaseType_t xTimer1Started, xTimer2Started;


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
  printf("System Initializing...\r\n");

  /*Timer creation*/
  xOneShotTimer = xTimerCreate("OneShot", mainONE_SHOT_TIMER_PERIOD, pdFALSE,
		  0, prvOneShotTimerCallback);
  xAutoReloadTimer = xTimerCreate("AutoReload", mainAUTO_RELOAD_TIMER_PERIOD,
		  pdTRUE, 0, prvAutoReloadTimerCallback);

  /*Starting Timers*/
  xTimer1Started = xTimerStart(xOneShotTimer, 0);
  xTimer2Started = xTimerStart(xAutoReloadTimer, 0);

  /*Start scheduler*/
  vTaskStartScheduler();

  while (1)
  {

  }

}

/* Timer Callback Function --------------------------------------------------*/
void prvOneShotTimerCallback(TimerHandle_t xTimer)
{
	static TickType_t xTimerNow;
	/*Obtain the current tick count*/
	xTimerNow = xTaskGetTickCount();

	printf("One-shot timer callback executing: %d \r\n", (int)xTimerNow);
}

void prvAutoReloadTimerCallback(TimerHandle_t xTimer)
{
	static TickType_t xTimerNow;
	/*Obtain the current tick count*/
	xTimerNow = xTaskGetTickCount();

	printf("Auto-reload timer callback executing: %d \r\n", (int)xTimerNow);

	timeoutCount++;
	if(timeoutCount == STOP_MARK)
	{
		printf("Stop mark reached %d \r\n", (int)xTimerNow);
		xTimerStop(xAutoReloadTimer, 0);

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



