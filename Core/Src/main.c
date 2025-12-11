/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "bsp_system.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  printf("[SYS] System Start\r\n");

  OLED_Init();
  printf("[OLED] Init Done\r\n");

  /* OLED 显示测试 */
  OLED_Clear();
  OLED_ShowString(1, 1, "Smart Car");
  OLED_ShowString(2, 1, "STM32F407");
  OLED_ShowString(3, 1, "Test:12345");
  OLED_ShowFloat(4, 1, 3.14, 1, 2);
  OLED_Flush();
  printf("[OLED] Display Test Done\r\n");

  Key_Init();
  printf("[KEY] Init Done\r\n");

  Motor_Init();           // ← 添加
  printf("[MOTOR] Init Done\r\n");
  Encoder_Init();         // ← 添加
  printf("[ENCODER] Init Done\r\n");

  scheduler_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
      Test_Function();
    /* USER CODE BEGIN 3 */
    scheduler_run();
    HAL_Delay(10);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
typedef enum {
      TEST_MOTOR_AUTO,     // 电机自动测试（正转-反转-停止循环）
      TEST_MOTOR_MANUAL,   // 按键手动控制
      TEST_ENCODER,        // 编码器速度显示
      TEST_ALL             // 综合测试
  } TestMode_t;

  static TestMode_t test_mode = TEST_ALL;
  static uint32_t test_step = 0;
  static uint32_t last_test_tick = 0;

  void Test_Function(void)
  {
      uint32_t now = HAL_GetTick();

      // 每500ms切换测试步骤
      if (now - last_test_tick >= 500) {
          last_test_tick = now;
          test_step++;

          switch(test_step % 8) {
              case 0:
                  printf("[TEST] 左电机正转300\r\n");
                  Motor_Set_Speed(&left_motor, 300);
                  Motor_Set_Speed(&right_motor, 0);
                  break;
              case 1:
                  printf("[TEST] 左电机反转-300\r\n");
                  Motor_Set_Speed(&left_motor, -300);
                  break;
              case 2:
                  printf("[TEST] 右电机正转300\r\n");
                  Motor_Set_Speed(&left_motor, 0);
                  Motor_Set_Speed(&right_motor, 300);
                  break;
              case 3:
                  printf("[TEST] 右电机反转-300\r\n");
                  Motor_Set_Speed(&right_motor, -300);
                  break;
              case 4:
                  printf("[TEST] 双电机正转500\r\n");
                  Motor_Set_Speed(&left_motor, 500);
                  Motor_Set_Speed(&right_motor, 500);
                  break;
              case 5:
                  printf("[TEST] 停止\r\n");
                  Motor_Stop(&left_motor);
                  Motor_Stop(&right_motor);
                  break;
              case 6:
                  printf("[TEST] 刹车\r\n");
                  Motor_Brake(&left_motor);
                  Motor_Brake(&right_motor);
                  break;
              case 7:
                  printf("[TEST] ========== 新循环 ==========\r\n");
                  break;
          }
      }

      // 实时打印编码器数据（每100ms）
      static uint32_t last_print = 0;
      if (now - last_print >= 100) {
          last_print = now;

          Encoder_Driver_Update(&left_encoder);
          Encoder_Driver_Update(&right_encoder);

          printf("[ENC] L:%d(%.1fcm/s)  R:%d(%.1fcm/s)\r\n",
                 left_encoder.count, left_encoder.speed_cm_s,
                 right_encoder.count, right_encoder.speed_cm_s);

          // OLED显示
          OLED_Clear();
          OLED_ShowString(1, 1, "L:");
          OLED_ShowSignedNum(1, 3, left_encoder.count, 5);
          OLED_ShowString(2, 1, "R:");
          OLED_ShowSignedNum(2, 3, right_encoder.count, 5);
          OLED_ShowString(3, 1, "LS:");
          OLED_ShowFloat(3, 4, left_encoder.speed_cm_s, 4, 1);
          OLED_ShowString(4, 1, "RS:");
          OLED_ShowFloat(4, 4, right_encoder.speed_cm_s, 4, 1);
          OLED_Flush();
      }
  }
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
