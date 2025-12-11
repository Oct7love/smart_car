//
// Created by 13615 on 2025/12/11.
//

#ifndef SMART_CAR_UART_DRIVER_H
#define SMART_CAR_UART_DRIVER_H
#include "bsp_system.h"
#define BUFFER_SIZE 128 // 缓冲区大小

int Uart_Printf(UART_HandleTypeDef *huart, const char *format, ...);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

#endif //SMART_CAR_UART_DRIVER_H