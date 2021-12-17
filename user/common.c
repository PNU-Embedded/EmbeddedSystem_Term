#include "common.h"

bool isNum(char* str) {
  while(*str) {
    if (isdigit(*str) == false)
      return false;
    ++str;
  }
  return true;
}

void sleep(int times) {
  for (int i = 0; i < times; ++i);
}