#include "command_handler.h"

bool isNum(char* str) {
  while(*str) {
    if (isdigit(*str) == false)
      return false;
    ++str;
  }
  return true;
}

void Command_Append(Command_Handler* command_handler, char ch) {
  if (command_handler->command_buffer_index == 100)
    return;
  command_handler->command_buffer[command_handler->command_buffer_index] = ch;
  ++(command_handler->command_buffer_index);
  command_handler->command_buffer[command_handler->command_buffer_index] = '\0';
}

void Command_Decide(Command_Handler* command_handler) {
  if (isNum(command_handler->command_buffer)) {
    command_handler->command = CMD_NUMBER;
  }
  else if (strcasecmp(command_handler->command_buffer, "clear")) {
    command_handler->command = CMD_CLEAR;
  }
  else if (strcasecmp(command_handler->command_buffer, "delete")) {
    command_handler->command = CMD_DELETE;
  }
  else if (strcasecmp(command_handler->command_buffer, "enter")) {
    command_handler->command = CMD_ENTER;
  }
  command_handler->command_buffer_index = 0;
}