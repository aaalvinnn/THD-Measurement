/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "stdio.h"
#include "FFT.h"
#include "WindowFunction.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
double ADC_Vol;
int i,adc;
#define m 11    //(=log2 N)即时序数组的以2为底的指数
#define Length 2048   //Length为时序数组的长度
#define Fs 100000     //采样频率
uint16_t ADC_Value[Length]; //储存ADC采集的数据
__IO uint8_t AdcConvEnd = 0;  //检测ADC是否采集完毕
// Complex Signal[Length];	//储存一组时序采样信号，用于FFT计算，以及作为FFT结果储存的缓冲区
float Distortion=0;
float DCAmp=0;
double pr[Length],pi[Length],fr[Length],fi[Length];
double window[Length];// 窗函数
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
	
  int X=0;		//蓝牙上位机屏幕显示横坐标
  int flag=0; //FFT标志位
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);
  // HAL_ADCEx_Calibration_Start(&hadc1);    //AD校准
  HAL_Delay(200);
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Value,Length);        //DMA发送数据
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	void sort(int, double*);
	hannWin(Length,window);	//加窗函数程序崩？
  while (1)
  {
    /* USER CODE END WHILE */
		HAL_Delay(200);
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_13);
    /* USER CODE BEGIN 3 */
    while(X<Length+1){    //设成Length+1是因为虽然X=Length时虽已经收集完采集结果，但需要再经过一个循环进行FFT计算.又因为当X=1024后不继续加了，所以陷入了空的死循环
      if(X<Length & flag<Length){
        ADC_Vol = ADC_Value[X]*3.3/4096;
				pr[X]=ADC_Vol * window[X];	//和窗函数混叠，以提高FFT正确率,这一句也不行？崩
        printf("*HX%dY%.4f",X++,ADC_Vol);	//用于serialchart波形串口调试
				
				
				
				/*原FFT变换传参
        Signal[flag].real = ADC_Vol;
        Signal[flag].imag = 0;
				*/
        flag++;
      }
      else if(flag == Length){
        flag = flag % Length;
				
				
				/* 新FFT变化 */
				kfft(pr,pi,Length,m,fr,fi);
				
				
				/*原FFT变化
        FFT(Signal,m);
        AmpSpectrum(Signal,m,&DCAmp,&Distortion);
				*/
				
				/*串口传输频域*/

				
        /*串口传输失真度*/
        sort(Length, pr);
        Distortion = sqrt((pr[Length-4]/Length/2)*(pr[Length-4]/Length/2) //二次谐波
        +(pr[Length-6]/Length/2)*(pr[Length-6]/Length/2)  //三次谐波
        +(pr[Length-8]/Length/2)*(pr[Length-8]/Length/2)  //四次谐波
        +(pr[Length-10]/Length/2)*(pr[Length-10]/Length/2)) //五次谐波
        /(pr[Length-2]/Length/2); //一次谐波频率分量幅值
        printf("*Z%.2f",Distortion*100);
				
        for(i=1;i<=(5);i++){
          printf("*GX%dY%.4f",i,pr[Length-i*2]/Length/2);
        }
      }
    }
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
  RCC_OscInitStruct.PLL.PLLM = 25;
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
/* 冒泡排序 从小到大
*   注意FFT后数组第一个元素是直流分量，在输入3.3/2=1.65V直流偏置时，直流偏置应该是最大的那一个量，排序后作为新数组中的最后一个元素；
*  同时其他的频率分量根据FFT计算的对称性，有两个相同的量
*/
void sort(int N,double *nums)
{	
    
    int i,j;
		double temp;//循环变量
	/******外层控制轮数********/
	for(i=0;i<N-1;i++)  
	{
	/******内层控制循环，每轮比较次数********/
		for(j=0;j<N-i-1;j++)
		{
			if(nums[j]>nums[j+1])//>升序<降序
			{
				temp=nums[j];
				nums[j]=nums[j+1];
				nums[j+1]=temp;
			}
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
