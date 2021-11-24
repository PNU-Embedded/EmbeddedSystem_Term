#include "command_handler.h"

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
  else if (strcasecmp(command_handler->command_buffer, "clear") == 0) {
    command_handler->command = CMD_CLEAR;
  }
  else if (strcasecmp(command_handler->command_buffer, "delete") == 0) {
    command_handler->command = CMD_DELETE;
  }
  else if (strcasecmp(command_handler->command_buffer, "enter") == 0) {
    command_handler->command = CMD_ENTER;
  }
}

void Command_Reset(Command_Handler* command_handler) {
  command_handler->command_buffer_index = 0;
}