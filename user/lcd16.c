#include "lcd16.h"

/*
  LCD16 PIN:
  C6
  D13, D16
  E0~E16
*/

static void LCD16_Sleep(int time){
  for (int i = 0; i < time; ++i);
}

static void LCD16_Write_Data(uint8_t LCD_Data)
{
  // D/C(RS) High
  GPIO_SetBits(GPIOD, GPIO_Pin_13);
  LCD16_Sleep(100);
  // CS(EN) High
  GPIO_SetBits(GPIOC, GPIO_Pin_6);
  // send Command
  GPIO_Write(GPIOE, LCD_Data);
  LCD16_Sleep(100);
  
  // CS(EN) Low
  GPIO_ResetBits(GPIOC, GPIO_Pin_6);
  LCD16_Sleep(5000);
}

static void LCD16_Write_Command(uint8_t LCD_Command)
{
  // D/C(RS) Low
  GPIO_ResetBits(GPIOD, GPIO_Pin_13);
  LCD16_Sleep(100);
  // CS(EN) High
  GPIO_SetBits(GPIOC, GPIO_Pin_6);
  LCD16_Sleep(100);
  
  // send Command
  GPIO_Write(GPIOE, LCD_Command);
  LCD16_Sleep(100);
  
  // CS(EN) Low
  GPIO_ResetBits(GPIOC, GPIO_Pin_6);
  LCD16_Sleep(5000);
}

static void LCD16_RCCInit(void) {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
}

static void LCD16_GPIOInit(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  /*DB00~DB16*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* LCD_RS  LCD_WR*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* LCD_CS */ // EN
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LCD16_Configure(void) {
  LCD16_RCCInit();
  LCD16_GPIOInit();
  
  GPIO_Write(GPIOE, 0);
  GPIO_ResetBits(GPIOD, GPIO_Pin_14);
  GPIO_ResetBits(GPIOD, GPIO_Pin_13);
  GPIO_ResetBits(GPIOC, GPIO_Pin_6);
  LCD16_Sleep(5000);
  
  // booting up
  LCD16_Write_Command(0x3c);
  LCD16_Sleep(1000);
  LCD16_Write_Command(0x0c);
  LCD16_Sleep(1000);
  LCD16_Write_Command(0x01);
  LCD16_Sleep(1000);
  LCD16_Write_Command(0x06);
  LCD16_Sleep(50000);
  
  LCD16_ShowPassword("____");
  LCD16_ShowMessage("Init Password");
}

void LCD16_ShowPassword(char* password) {
  LCD16_Write_Command(0x80);
  
  int password_len = strlen(password);
  for (int i = 0; i < 6; ++i)
    LCD16_Write_Data(' ');
  while(*password != '\0'){
    LCD16_Write_Data(*password);
    ++password;
  }
  for (int i = 0; i < 4 - password_len; ++i)
    LCD16_Write_Data('_');
  for (int i = 0; i <6; ++i)
    LCD16_Write_Data(' ');
}

void LCD16_ShowMessage(char* message) {
  LCD16_Write_Command(0xc0);
  
  int message_len = strlen(message);
  for (int i = 0; i < 16 - message_len; ++i)
    LCD16_Write_Data(' ');
  
  while(*message != '\0'){
    LCD16_Write_Data(*message);
    ++message;
  }
}