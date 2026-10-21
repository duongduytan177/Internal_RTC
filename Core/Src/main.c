/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "screen.h"
#include <stdio.h>
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
Screen_t screen = SCREEN_CLOCK;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* Bien tam khi chinh sua gio thuc */
int editHour = 0;
int editMinute = 0;
int editSecond = 0;
int editTimeField = 0; /* 0: Gio, 1: Phut, 2: Giay */

/* Thong so bao thuc */
int alarmHour = 6;
int alarmMinute = 30;
uint8_t alarmEnable = 1;

/* Bien tam khi chinh sua bao thuc */
int editAlarmHour = 6;
int editAlarmMinute = 30;
uint8_t editAlarmEnable = 1;
int editAlarmField = 0; /* 0: Gio, 1: Phut, 2: Bat/Tat */

/* Trang thai bao thuc */
uint8_t alarmRinging = 0;
uint8_t alarmTriggered = 0;
uint32_t alarmStartMillis = 0;

/*menu*/
int menuItem = 0;
/* Quan ly chu ky cap nhat man hinh */
uint32_t lastDisplayUpdate = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*GHI THOI GIAN VAO RTC*/
void Clock_SetTime(uint8_t h, uint8_t m, uint8_t s)
{
    RTC_TimeTypeDef time = {0};

    time.Hours = h;
    time.Minutes = m;
    time.Seconds = s;

    if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}

/*DOC THOI GIAN TU RTC*/
void Clock_GetTime(uint8_t *h, uint8_t *m, uint8_t *s)
{
    RTC_TimeTypeDef time = {0};

    if (HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN) == HAL_OK)
    {
        *h = time.Hours;
        *m = time.Minutes;
        *s = time.Seconds;
    }
}

/*CAP NHAT GIAO DIEN MÀN HÌNH (DONG BO VOI MODULE SCREEN)*/
void updateDisplay(void)
{
    if (alarmRinging)
    {
        Screen_DrawAlarmRinging(alarmHour, alarmMinute);
        return;
    }

    switch (screen)
    {
        case SCREEN_CLOCK:
        {
            uint8_t h, m, s;
            Clock_GetTime(&h, &m, &s);
            Screen_DrawClock(h, m, s, alarmHour, alarmMinute, alarmEnable);
            break;
        }

        case SCREEN_MENU:
            Screen_DrawMenu(menuItem);
            break;

        case SCREEN_SET_TIME:
            Screen_DrawSetTime(editHour, editMinute, editSecond, editTimeField);
            break;

        case SCREEN_SET_ALARM:
            Screen_DrawSetAlarm(editAlarmHour, editAlarmMinute, editAlarmEnable, editAlarmField);
            break;
    }
}

/*KIEM TRA THOI GIAN KICH HOAT BAO THUC*/
void checkAlarm(void)
{
    uint8_t h, m, s;

    if (!alarmEnable)
    {
        alarmTriggered = 0;
        return;
    }

    Clock_GetTime(&h, &m, &s);

    if ((h != alarmHour) || (m != alarmMinute))
    {
        alarmTriggered = 0;
    }

    /* DUNG GIO BAO THUC */
    if ((h == alarmHour) && (m == alarmMinute) && (s <= 2) && (!alarmTriggered))
    {
        alarmTriggered = 1;
        alarmRinging = 1;
        alarmStartMillis = HAL_GetTick();
        screen = SCREEN_CLOCK;
        Screen_DrawAlarmRinging(alarmHour, alarmMinute);
    }
}

/*DIEU KHIEN AM THANH COI BUZZER THEO NHIP*/
void updateBuzzer(void)
{
    uint32_t t;

    if (!alarmRinging)
    {
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_RESET);
        return;
    }

    t = (HAL_GetTick() - alarmStartMillis) % 1500;

    /* BIP 1 */
    if (t < 150)
    {
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_SET);
    }
    /* NGHI NGAN */
    else if (t < 250)
    {
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_RESET);
    }
    /* BIP 2 */
    else if (t < 400)
    {
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_SET);
    }
    /* NGHI NGAN */
    else if (t < 500)
    {
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_RESET);
    }
    /* BIP 3 */
    else if (t < 650)
    {
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_SET);
    }
    /* NGHI DAI */
    else
    {
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_RESET);
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(300);
  OLED_Init();
  Button_Init();
  HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_RESET);
  Screen_ShowWelcome();
  screen = SCREEN_CLOCK;
  updateDisplay();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Encoder_Handle();
	  Button_Handle();
	  checkAlarm();
	  updateBuzzer();
	  /* Cap nhat dong ho sau moi 200ms */
	  if ((screen == SCREEN_CLOCK) && (!alarmRinging))
	  {
		  if ((HAL_GetTick() - lastDisplayUpdate) >= 200)
		  {
			  lastDisplayUpdate = HAL_GetTick();
	          uint8_t h, m, s;
	          Clock_GetTime(&h, &m, &s);
			  Screen_DrawClock(h, m, s, alarmHour, alarmMinute, alarmEnable);
		  }
	  }

	  HAL_Delay(1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
#ifdef USE_FULL_ASSERT
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
