#include "password_handler.h"

bool Password_Update(Password_Handler* password_handler) {
  if (password_handler->current_buffer_index != 4)
    return false;
  
  for (int i = 0; i < 4; ++i) 
    password_handler->password[i] = password_handler->current_buffer[i];
  password_handler->current_buffer[0] = '\0';
  password_handler->current_buffer_index = 0;
  return true;
}

bool Password_Enter(Password_Handler* password_handler) {
  if (password_handler->current_buffer_index != 4)
    return false;
  
  for (int i = 0; i < 4; ++i) {
    if (password_handler->password[i] != password_handler->current_buffer[i])
      return false;
  }
  password_handler->current_buffer[0] = '\0';
  password_handler->current_buffer_index = 0;
  return true;
}

void Password_Type(Password_Handler* password_handler, char ch) {
  if (password_handler->current_buffer_index == 4)
    return;
  password_handler->current_buffer[password_handler->current_buffer_index] = ch;
  ++(password_handler->current_buffer_index);
  password_handler->current_buffer[password_handler->current_buffer_index] = '\0';
}

void Password_Delete(Password_Handler* password_handler) {
  if (password_handler->current_buffer_index > 0) {
    password_handler->current_buffer_index--;
    password_handler->current_buffer[password_handler->current_buffer_index] = '\0';
  }
}

void Password_Reset(Password_Handler* password_handler) {
  password_handler->current_buffer[0] = '\0';
  password_handler->current_buffer_index = 0;
  for (int i = 0; i < 4; ++i) 
    password_handler->password[i] = 0;
}