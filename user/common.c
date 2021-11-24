#include "common.h"

bool isNum(char* str) {
  while(*str) {
    if (isdigit(*str) == false)
      return false;
    ++str;
  }
  return true;
}

void Bluetooth_SendString(char* str) {
  printf("%s\n", str);
  USART_SendData(USART2, (uint16_t)255);
  sleep(100000);
  while(*str != '\0') {
    USART_SendData(USART2, *str);
    sleep(100000);
    ++str;
  }
  sleep(100000);
  USART_SendData(USART2, 0);
}

void sleep(int times) {
  for (int i = 0; i < times; ++i);
}