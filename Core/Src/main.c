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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "w25qxx.h"
#include "w25qxxConf.h"
#include "flash.h"
#include "string.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define  ADDRESS_DATA_HOST 0x08004000
#define  ADDRESS_DATA_PORT 0x08008000
#define  ADDRESS_DATA_MQTT_PORT 0x0800C000
#define  ADDRESS_DATA_USERNAME 0x08010000
#define  ADDRESS_DATA_PASSWORD 0x08014000
#define  ADDRESS_DATA_DEVICE_PROFILE 0x08018000

#define bufferSize 20
char arr_r[20]="";
char config_buffers[20]={};
char Pass_Config_1[6]="config";
char Rx_Config_1[bufferSize];

char Pass_Word[4]="abcd";
char Rx_Pass_Config_2[4];

char Host[]="seval.ddns.net";
char Port[]="8080";
char MQTT_port[]="1883";
char Username[]="Device001";
char Password[]="test";
char Device_Profile[]="SV1";
char Rx_config[20];
int flag_config_1 = 0;
size_t sizeOfValueCf[6]={};

const char* configNames[] = {
    "HOST",
    "PORT",
    "MQTT_PORT",
    "USERNAME",
    "PASSWORD",
    "DEVICE_PROFILE"
};
uint32_t configAddresses[] = {
    ADDRESS_DATA_HOST,
    ADDRESS_DATA_PORT,
    ADDRESS_DATA_MQTT_PORT,
    ADDRESS_DATA_USERNAME,
    ADDRESS_DATA_PASSWORD,
    ADDRESS_DATA_DEVICE_PROFILE
};

char Profile[4]={};
char Device_Profile_1[]="SV1";
char Device_Profile_2[]="SV2";
char Device_Profile_3[]="SV3";

#define Register 100
#define FAIL_SIZE 10
uint64_t number_byte1=0;
uint64_t number_byte2=0;
uint64_t number_byte3=0;
uint64_t number_page=0;
int flag_read = 0;
int flag_EraseChip = 0;
int mode=1;
//SV1: (16bytes)
uint16_t PI_1 = 8000;
uint16_t PI_2 = 7000;
uint8_t DI_1_485[9]={0x01, 0x03, 0x04, 0x01, 0x02, 0xFF, 0x33, 0x5B , 0xEA};
uint16_t AI_1_420=1500;
uint16_t AI_2_420=1500;
uint16_t AI_3_420=1500;
uint16_t AI_1_485=1500;
uint16_t AI_2_485=2000;
uint16_t AI_3_485=500;
uint16_t AI_4_485=400;
uint8_t Battery=99;
uint8_t Write1[16];
uint8_t Write2[18];
uint8_t arrtest2[256];

uint8_t Read_DI_1_485[9];
 uint32_t Page_address = 0x00000000;
 uint32_t Number_byte_address = 0x00000000;
 int maxRetryCount = 3;
 int retryCount = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		flag_config_1 = 1;
		uint8_t	 receivedChar;
		printf("+Start Config (Enter)\n");
		size_t index = 0;
		do {
			HAL_UART_Receive(&huart1, &receivedChar, 1, 2000);
			if (receivedChar == '\n') {
				printf("+Pass Config:\n");
				break;
			}
		} while (1);
		memset(Rx_Config_1,'\0',sizeof(Rx_Config_1));
		do {
			HAL_UART_Receive(&huart1, &receivedChar, 1, 2000);
			if (receivedChar == '\n' || index >= bufferSize - 1) {
				Rx_Config_1[index] = '\0'; // Kết thúc chuỗi
				break;
			}
			Rx_Config_1[index] = receivedChar;
			index++;
		} while (1);
		size_t len = strlen(Rx_Config_1);
		if (len > 0 && Rx_Config_1[len - 1] == '\r') {
			Rx_Config_1[len - 1] = '\0';
		}
		if (strcmp(Rx_Config_1,Pass_Config_1)==0) {
				printf("STM32 PassConfig:Done\n");

				while(retryCount<maxRetryCount){
					printf("+Enter Password:\n");
					flag_config_1=1;
					index = 0;
					memset(Rx_Config_1,'\0',sizeof(Rx_Config_1));
					do {
						HAL_UART_Receive(&huart1, &receivedChar, 1, 2000);
						if (receivedChar == '\n' || index >= bufferSize - 1) {
							Rx_Config_1[index] = '\0'; // Kết thúc chuỗi
							break;
						}
						Rx_Config_1[index] = receivedChar;
						index++;
					} while (1);

					size_t len1 = strlen(Rx_Config_1);
					if (len1 > 0 && Rx_Config_1[len1 - 1] == '\r') {
						Rx_Config_1[len1 - 1] = '\0';
					}
					if (len1<4) {
						flag_config_1 = 0;
					}
					else {
						for (int i = 0; i < len1-1; ++i) {
							if (Rx_Config_1[i]!=Pass_Word[i]) {
								flag_config_1 = 0;
								break;
							}
						}
					}

					if (flag_config_1 == 0) {
						   printf("STM32 Password:Fail\n");
						   retryCount++;
					   }
					else {
						   printf("STM32 Password:Done\n");
							//Xoa vung nho
						   for (int i = 0; i < 6; ++i) {
							Flash_Erase(configAddresses[i]);
						   }
							for (int i = 0; i < 6; ++i) {
								ConfigAndPrint(configNames[i], configAddresses[i]);
								sizeOfValueCf[i] = index;
							}
							PrintConfig_All();
							flag_EraseChip=1;
							number_byte1=0;
							number_byte2=0;
							number_byte3=0;
							number_page=0;
							retryCount=10;
					   }
					}
					retryCount=0;
				}
		        else {
					printf("STM32 PassConfig:Fail\n");
				}
		HAL_UART_Receive_IT(&huart1, (uint8_t*)Rx_Config_1, bufferSize);
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
  W25qxx_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  for (int i = 0; i < 6; ++i) {
	Flash_Erase(configAddresses[i]);
  }
// in vao flash
  Flash_Write_Array(ADDRESS_DATA_HOST, (uint8_t*)Host, sizeof(Host));
  Flash_Write_Array(ADDRESS_DATA_PORT, (uint8_t*)Port, sizeof(Port));
  Flash_Write_Array(ADDRESS_DATA_MQTT_PORT, (uint8_t*)MQTT_port, sizeof(MQTT_port));
  Flash_Write_Array(ADDRESS_DATA_USERNAME, (uint8_t*)Username, sizeof(Username));
  Flash_Write_Array(ADDRESS_DATA_PASSWORD, (uint8_t*)Password, sizeof(Password));
  Flash_Write_Array(ADDRESS_DATA_DEVICE_PROFILE, (uint8_t*)Device_Profile, sizeof(Device_Profile));
  //in ra man hinh
  PrintConfig_All();

  HAL_UART_Receive_IT(&huart1, (uint8_t*)Rx_Config_1, bufferSize);
  flag_EraseChip=1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (memcmp(Device_Profile_1, Profile, 3) == 0) {
		  if (flag_EraseChip==1) {
			  flag_EraseChip=0;
			  printf("Xoa chip\n");
//			  W25qxx_EraseChip();
		}
	 		  Write1[0]=PI_1;
	 		  Write1[1]=(PI_1>>8)&0xFF;
	 		  Write1[2]=PI_2;
	 		  Write1[3]=(PI_2>>8)&0xFF;
	 		  for (int i = 0; i < 9; i++) {
	 			Write1[i + 4] = DI_1_485[i];
	 		  }
	 		  Write1[13]=AI_2_420;
	 		  Write1[14]=(AI_2_420>>8)&0xFF;
	 		  Write1[15]=Battery;
			  W25qxx_WritePage(Write1, 0x00000000+number_page, 0x00000000+number_byte1, sizeof(Write1));
			  number_byte1+=16;
			  if (number_byte1 >=256 ) {
				number_page++;
				number_byte1=0;
				  }
	 			  printf("Hello Device_Profile_1\n");
	 	  }
	 	  else if (memcmp(Device_Profile_2, Profile, 3) == 0) {
	 		  if (flag_EraseChip==1) {
	 			  flag_EraseChip=0;
	 			  printf("Xoa chip\n");
	 			  W25qxx_EraseChip();
	 		}

	 		  for (int i = 0; i < 9; i++) {
	 			Write2[i] = DI_1_485[i];
	 		  }
	 		Write2[9]=AI_1_485;
	 		Write2[10]=(AI_1_485>>8)&0xFF;
	 		Write2[11]=AI_2_485;
	 		Write2[12]=(AI_2_485>>8)&0xFF;
	 		Write2[13]=AI_3_485;
	 		Write2[14]=(AI_3_485>>8)&0xFF;
	 		Write2[15]=AI_4_485;
	 		Write2[16]=(AI_4_485>>8)&0xFF;
	 		Write2[17]=Battery;
			  W25qxx_WritePage(Write2, 0x00000000+number_page, 0x00000000+number_byte2, sizeof(Write2));
			  number_byte2+=18;
			  if (number_byte2 >256 ) {
				number_page++;
				number_byte2=0;
			  }

	 		  printf("Hello Device_Profile_2\n");
	 	  }
	 	  else if (memcmp(Device_Profile_3, Profile, 3) == 0) {
	 		  if (flag_EraseChip==1) {
	 			  flag_EraseChip=0;
	 			  printf("Xoa chip\n");
	 			  W25qxx_EraseChip();
	 		}

	 		  Write2[0]=PI_1;
	 		  Write2[1]=(PI_1>>8)&0xFF;
	 		  for (int i = 0; i < 9; i++) {
	 			Write2[i+2] = DI_1_485[i];
	 		  }
	 			Write2[11]=AI_1_420;
	 			Write2[12]=(AI_1_420>>8)&0xFF;
	 			Write2[13]=AI_2_420;
	 			Write2[14]=(AI_2_420>>8)&0xFF;
	 			Write2[15]=AI_3_420;
	 			Write2[16]=(AI_3_420>>8)&0xFF;
	 			Write2[17]=Battery;
			  W25qxx_WritePage(Write2, 0x00000000+number_page, 0x00000000+number_byte2, sizeof(Write2));
			  number_byte2+=18;
			  if (number_byte2 >256 ) {
				number_page++;
				number_byte2=0;
			  }

	 		  printf("Hello Device_Profile_3\n");
	 		}
	 	  else {
			printf("Device_Profile Fail\n");
		}
	  HAL_Delay(1000);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void ConfigAndPrint(const char *configName, uint32_t flashAddress) {

    uint8_t str2[50]="";
    sprintf(str2,"+Enter %s:\n", configName);
    HAL_UART_Transmit(&huart1, str2, sizeof(str2), 500);
    memset(str2, 0, sizeof(str2));
    uint8_t receivedChar;
    size_t index = 0;

    do {
        HAL_UART_Receive(&huart1, &receivedChar, 1, 2000);
        if (receivedChar == '\n' || index >= bufferSize - 1) {
            Rx_Config_1[index] = '\0'; // Kết thúc chuỗi
            break;
        }
        Rx_Config_1[index] = receivedChar;
        index++;
    } while (1);
    sprintf(str2,">%s\n", Rx_Config_1);
    HAL_UART_Transmit(&huart1, str2, sizeof(str2), 500);
    memset(str2, 0, sizeof(str2));

    Flash_Write_Array(flashAddress, (uint8_t*)&Rx_Config_1, index);
    memset(Rx_Config_1, 0, sizeof(Rx_Config_1));
    memset(arr_r, 0, sizeof(arr_r));

    sprintf(str2,"Done\n");
    HAL_UART_Transmit(&huart1, str2, sizeof(str2), 500);
    memset(str2, 0, sizeof(str2));

}

void PrintConfig(const char *value_config, uint32_t flashAddress,size_t value_size){
    Flash_Read_Array(flashAddress, (uint8_t*)&arr_r, value_size);
    uint8_t str2[50]="";
    sprintf(str2,"%s: %s\n",value_config, arr_r);
    HAL_UART_Transmit(&huart1, str2, sizeof(str2), 500);
    memset(arr_r, 0, sizeof(arr_r));
}
void PrintConfig_All(){
	  PrintConfig("HOST", ADDRESS_DATA_HOST,sizeof(Host));
	  PrintConfig("PORT", ADDRESS_DATA_PORT,sizeof(Port));
	  PrintConfig("MQTT_PORT", ADDRESS_DATA_MQTT_PORT,sizeof(MQTT_port));
	  PrintConfig("USERNAME", ADDRESS_DATA_USERNAME,sizeof(Username));
	  PrintConfig("PASSWORD", ADDRESS_DATA_PASSWORD,sizeof(Password));
	  PrintConfig("DEVICE_PROFILE", ADDRESS_DATA_DEVICE_PROFILE,sizeof(Device_Profile));
	  Flash_Read_Array(ADDRESS_DATA_DEVICE_PROFILE, (uint8_t*)&Profile, sizeof(Profile));
}
#if defined(__GNUC__)
int _write(int fd, char * ptr, int len)
{
  HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
  return len;
}
#elif defined (_ICCARM_)
#include "LowLevelIOInterface.h"
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  HAL_UART_Transmit(&huart1, (uint8_t *) buffer, size, HAL_MAX_DELAY);
  return size;
}
#elif defined (__CC_ARM)
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
#endif
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
