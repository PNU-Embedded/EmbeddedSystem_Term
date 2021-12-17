#include "accelerator.h"

/*
  x좌표만 dma로 받아오고, z좌표는 요청 때 마다 좌표값 읽어옴.
  dma2는 adc3와 연결이 가능한데 보드에서 adc3를 지원하지 않기때문에
  dma2 - adc2 연결이 보드 자체에서 불가능하기 때문하여 dma1-adc1만 연결 함.
  최대한 배운거 다 활용하고자 dma사용.
*/

#define ACCELERATOR_GPIO GPIOA
#define ACCELERATOR_GPIO_X_PIN GPIO_Pin_0
#define ACCELERATOR_GPIO_Z_PIN GPIO_Pin_1
#define ACCELERATOR_ADC_X_CH ADC_Channel_0
#define ACCELERATOR_ADC_Z_CH ADC_Channel_1

static void Accelerator_RCCInit(void);
static void Accelerator_GPIOInit(void);
static void Accelerator_ADCInit(void);
static void Accelerator_DMAInit(void);

volatile uint16_t accelerator_x;

static void Accelerator_RCCInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}
static void Accelerator_GPIOInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* X축 */
  GPIO_InitStructure.GPIO_Pin = ACCELERATOR_GPIO_X_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(ACCELERATOR_GPIO, &GPIO_InitStructure);
  
  /* Z축 */
  GPIO_InitStructure.GPIO_Pin = ACCELERATOR_GPIO_Z_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(ACCELERATOR_GPIO, &GPIO_InitStructure);
}

static void Accelerator_ADCInit() {
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;

  /* ADC1 */
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ACCELERATOR_ADC_X_CH, 1, ADC_SampleTime_239Cycles5);
  ADC_Cmd(ADC1, ENABLE);
  
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1));
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);
  
  /* ADC2 */
  ADC_Init(ADC2, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC2, ACCELERATOR_ADC_Z_CH, 1, ADC_SampleTime_239Cycles5);
  ADC_Cmd(ADC2, ENABLE);
  
  ADC_ResetCalibration(ADC2);
  while(ADC_GetResetCalibrationStatus(ADC2));
  ADC_StartCalibration(ADC2);
  
  while(ADC_GetCalibrationStatus(ADC2));
  ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}

static void Accelerator_DMAInit(void) {
  DMA_InitTypeDef DMA_InitStructure;
  
  /* DMA1 - ADC1 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&accelerator_x;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE); 
}

void Accelerator_Configure(void) {
  Accelerator_RCCInit();
  Accelerator_GPIOInit();
  Accelerator_ADCInit();
  Accelerator_DMAInit();
}

uint16_t Accelerator_Get_X(void) {
  return accelerator_x / 10;
}

uint16_t Accelerator_Get_Z(void) {
  return ADC_GetConversionValue(ADC2) / 10;
}