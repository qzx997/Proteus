/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
// PWM    V       speed  
// 100    1.24      74
// 10			0.59			34
unsigned char PWM = 50;	
float speed_target = 0.0, speed_now = 0, speed_last;
char feedback = 0;
char dir_flag = 0;
GPIO_PinState key_flag;
unsigned char SA1_value, SB1_value, SC1_value;
GPIO_PinState SA1, SB1, SC1;
char AH, AL, BH, BL, CH, CL;

// ADC
#define NPT 256                                 //????
uint32_t adc_buf[NPT]={0};                      //????ADC???????
uint16_t ADC_value;

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
  MX_TIM2_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
	
	HAL_ADC_Start_DMA(&hadc1, adc_buf, NPT);  /*??ADC?DMA??,???????ADC??  12??ADC??0-4095 */
  HAL_TIM_Base_Start(&htim3);               /*?????,????????ADC*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		
		SA1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
		SB1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
		SC1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
		
		if(SA1 == GPIO_PIN_SET) SA1_value = 1;
		else SA1_value = 0;
		if(SB1 == GPIO_PIN_SET) SB1_value = 1;
		else SB1_value = 0;
		if(SC1 == GPIO_PIN_SET) SC1_value = 1;
		else SC1_value = 0;
		
		feedback = ((unsigned char)(SA1_value)<<2) + ((unsigned char)(SB1_value)<<1) + (unsigned char)(SC1_value);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
//    uint16_t flag;
	ADC_value = HAL_ADC_GetValue(&hadc1);
	speed_last = speed_now;
	speed_now = (float)ADC_value * 5.0/65535.0 * 74.0/1.24;
//  HAL_ADC_Stop_DMA(&hadc1);   //??ADC?DMA??
//    flag = 1;                   //??ADC_DMA????
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		static int motor_cnt = 0, pwm_cnt = 0;
		motor_cnt ++; 
		
		key_flag = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
		if(key_flag == GPIO_PIN_SET)
		{
			dir_flag = ~dir_flag;
		}
		
		if(motor_cnt == 50)
		{
			motor_cnt = 0;
			pwm_cnt++;
			if(pwm_cnt >= 100)
			{
				pwm_cnt = 0;
			}
			
			if( pwm_cnt > PWM )
			{
				if(!dir_flag)
				 {
				
					switch(feedback)//
					{
						case 0x06:  AH=0;AL=0;BH=1;BL=0;CH=0;CL=0;break;
						case 0x02:	AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
						case 0X03:	AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
						case 0x01:	AH=0;AL=0;BH=0;BL=1;CH=0;CL=0;break;
						case 0x05:	AH=1;AL=0;BH=0;BL=0;CH=0;CL=0;break;
						case 0x04:  AH=0;AL=0;BH=0;BL=0;CH=0;CL=1;break;
						 default: break;
					}
				}	
				else
				{
					switch(feedback)//
					{
						case 0x06:  AH=0;AL=0;BH=0;BL=1;CH=0;CL=0;break;
						case 0x02:	AH=0;AL=0;BH=0;BL=0;CH=0;CL=1;break;
						case 0X03:	AH=0;AL=0;BH=0;BL=0;CH=0;CL=1;break;
						case 0x01:	AH=0;AL=0;BH=1;BL=0;CH=0;CL=0;break;
						case 0x05:	AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
						case 0x04:  AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
						default: break;
					}
				}
			}
			else
			{
					
				if(!dir_flag)
				 {
				
					switch(feedback)//
					{
						case 0x06:  AH=0;AL=1;BH=1;BL=0;CH=0;CL=0;break;
						case 0x02:	AH=0;AL=1;BH=0;BL=0;CH=1;CL=0;break;
						case 0X03:	AH=0;AL=0;BH=0;BL=1;CH=1;CL=0;break;
						case 0x01:	AH=1;AL=0;BH=0;BL=1;CH=0;CL=0;break;
						case 0x05:	AH=1;AL=0;BH=0;BL=0;CH=0;CL=1;break;
						case 0x04:  AH=0;AL=0;BH=1;BL=0;CH=0;CL=1;break;
						default: break;
					}
				}	
				else
				{
					switch(feedback)//
					{
						case 0x06:  AH=1;AL=0;BH=0;BL=1;CH=0;CL=0;break;
						case 0x02:	AH=1;AL=0;BH=0;BL=0;CH=0;CL=1;break;
						case 0X03:	AH=0;AL=0;BH=1;BL=0;CH=0;CL=1;break;
						case 0x01:	AH=0;AL=1;BH=1;BL=0;CH=0;CL=0;break;
						case 0x05:	AH=0;AL=1;BH=0;BL=0;CH=1;CL=0;break;
						case 0x04:  AH=0;AL=0;BH=0;BL=1;CH=1;CL=0;break;
						default: break;
					}
				}
			}
			if(AH == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			
			if(AL == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
			
			if(BH == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
			
			if(BL == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			
			if(CH == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
			
			if(CL == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		}
		
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
