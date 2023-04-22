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
#define m 11   //(=log2 N)即时序数组的以2为底的指数
#define Length 2048   //Length为时序数组的长度
#define Fs 500000     //采样频率 根据cubeMX来设置			//太奇怪了，单片机采样频率的确定，测量大概在400k~500k，但怎么都改不了！
#define F 1000    //输入信号基频
uint16_t ADC_Value[Length+2]; //储存ADC采集的数据
__IO uint8_t AdcConvEnd = 0;  //检测ADC是否采集完毕
float Distortion=0;
float DCAmp=0;
double pr[Length],pi[Length],fr[Length],fi[Length];
double window[Length];// 窗函数
double Fstep = (double)Fs/Length; //频率分辨率
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
  // +-int flag=0; //计数标志位
  int flag1[5]; //储存谐波下标
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
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);
	
	// HAL_TIM_Base_Start_IT(&htim9);
	
	HAL_Delay(200);
  // HAL_ADCEx_Calibration_Start(&hadc1);    //AD校准
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Value,Length+2);        //DMA发送数据
	while(!AdcConvEnd);		//等待AD采集完成
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	void sort(int, double*, int*);
	void freadd(int N,double *nums, int flag1[], int n);
	hannWin(Length,window);	//加窗函数
	/* USER CODE BEGIN 3 */
	/* LED0闪烁一下，表示程序正常运行 */ 
	HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_13);
	for(X=0;X<Length;X++){
		/* 三次移动平均 */
		for(i=0;i<3;i++){
			ADC_Vol+=ADC_Value[X+i];
		}
    ADC_Vol = (double)ADC_Vol*3.3/4095/3;
		pr[X]=ADC_Vol * window[X];	//和窗函数混叠，以提高FFT正确率
    if(X<200)		printf("*HX%dY%.4f",X,ADC_Vol);	//用于serialchart波形串口调试
    }
		kfft(pr,pi,Length,m,fr,fi);
		/*
		for(X=0;X<Length;X++){
			printf("*HX%dY%.4f",X,pr[X]);
		}
		*/
    /*串口传输失真度*/
    sort(Length, pr,flag1);
		freadd(Length, pr,flag1,3);	//减少频谱泄露，累加周围的3次频域
    Distortion = sqrt((pr[flag1[1]])*(pr[flag1[1]]) //二次谐波
    +(pr[flag1[2]])*(pr[flag1[2]])  //三次谐波
    +(pr[flag1[3]])*(pr[flag1[3]])  //四次谐波
    +(pr[flag1[4]])*(pr[flag1[4]])) //五次谐波
    /(pr[flag1[0]]); //一次谐波频率分量幅值
    printf("*Z%.2f",Distortion*100);
		/* 输出五次谐波归一化幅值 */
		printf("*A%.4f",pr[flag1[0]]/pr[flag1[0]]);
		printf("*B%.4f",pr[flag1[1]]/pr[flag1[0]]);
		printf("*C%.4f",pr[flag1[2]]/pr[flag1[0]]);
		printf("*D%.4f",pr[flag1[3]]/pr[flag1[0]]);
		printf("*E%.4f",pr[flag1[4]]/pr[flag1[0]]);
  while (1)
  {
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* 冒泡排序 从小到大
*   注意FFT后数组第一个元素是直流分量，在输入3.3/2=1.65V直流偏置时，直流偏置应该是最大的那一个量，排序后作为新数组中的最后一个元素；
*  同时其他的频率分量根据FFT计算的对称性，有两个相同的量
*/
void sort(int N,double *nums, int flag1[])
{	
    int i,j=0;
  /* 寻找极大值下标 */
	/* 先找出基波（一次谐波）频率最大值点 */
	flag1[0]=5;
	/* 只查找前半部分，从i=2开始是因为排除直流分量（i=0）以及因加窗导致的直流分量频域附近的影响（i=2）*/
	for(i=2;i<(Length)>>1;i++)		
	{
		if(pr[i+1]>=pr[flag1[0]])
		{
			flag1[0]=i+1;
		}
	}
  /* 然后开始找五次谐波内的极大值点 */
	for(j=0;j<4;j++)
	{
		/* 因谐波频率都是基波频率的整数倍，故先确定小范围寻找极大值的中心点 */
		// flag1[j+1]=flag1[j]+(int)F/Fstep;
		flag1[j+1] = flag1[0]*(j+2);			//因为单片机采样频率难以确定，所以不使用通过采样频率确定极大值下标的方法
		/* 一次搜寻包括中心频率点在内的附近10个点 */
		for(i=0;i<9;i++)		
		{
			if(pr[flag1[j+1]+i-4]>=pr[flag1[j+1]])
			{
				flag1[j+1]=flag1[j+1]+i-4;
			}
		}
	}
}
/* 频谱累积 */
/* 其中n为累加的次数 */
void freadd(int N,double *nums, int flag1[], int n)
{
	int i,j;
	for(i=0;i<5;i++)
	{
		for(j=0;j<n;j++)
		{
			nums[flag1[i]]+=nums[flag1[i]-(int)n/2+j];
		}
	}
}	

/* 中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim == &htim9){
    static int i=0;
    if(i<3){

    }
    else{
		  printf("TIMER TEST");	
			HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_13);
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
