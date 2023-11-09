#ifndef _UART_H_
#define _UART_H_
#include "main.h"

#define MAX_FIFO_SIZE 128


struct FIFO_Typedef
{
    uint8_t buf[MAX_FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
};

void upload(uint8_t *data);
void UART1_Start_Receive(void);
void UART3_Start_Receive(void);
int uart_printf(UART_HandleTypeDef *huart, const char *fmt, ...);
uint8_t cheak_sum(uint8_t *data, uint8_t packNum);

#define debug(...)  uart_printf(&huart1,__VA_ARGS__)

#endif

