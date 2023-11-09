#include "UART.h"
#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>

struct FIFO_Typedef UART1_FIFO = {0, 0, 0};
struct FIFO_Typedef UART3_FIFO = {0, 0, 0};
uint8_t UART_Number = 0;

void UART1_Start_Receive(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1_FIFO.buf, MAX_FIFO_SIZE);
}

void UART3_Start_Receive(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, UART3_FIFO.buf, MAX_FIFO_SIZE);
}

int uart_printf(UART_HandleTypeDef *huart, const char *fmt, ...)
{
    va_list arg;
    int cnt;
    char buffer[256];

    va_start(arg, fmt);
    cnt = vsprintf(buffer, fmt, arg);
    va_end(arg);

    while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET)
        ;

    HAL_UART_Transmit_DMA(huart, (uint8_t *)buffer, cnt);

    return cnt;
}

void upload(uint8_t *data)
{
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET)
        ;
    HAL_UART_Transmit_DMA(&huart1, data, 8);

    while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET)
        ;
    HAL_UART_Transmit_DMA(&huart3, data, 8);
}

extern osSemaphoreId_t UART_RX_FinshHandle;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart3_rx;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        UART1_FIFO.head = Size;
        // if (__HAL_DMA_GET_COUNTER(&hdma_usart1_rx) == 0)
        {
            UART_Number = 1;
            osSemaphoreRelease(UART_RX_FinshHandle);
        }
    }
    else if (huart->Instance == USART3)
    {
        UART3_FIFO.head = Size;
        // if (__HAL_DMA_GET_COUNTER(&hdma_usart3_rx) == 0)
        {
            UART_Number = 3;
            osSemaphoreRelease(UART_RX_FinshHandle);
        }
    }
}

uint8_t cheak_sum(uint8_t *data, uint8_t packNum)
{
    uint16_t temp = data[0 + packNum * 8] + data[1 + packNum * 8] + data[2 + packNum * 8] + data[3 + packNum * 8] +
                    data[4 + packNum * 8] + data[5 + packNum * 8] + data[7 + packNum * 8];
    uint8_t value = (temp % 0xFF) & 0xFF;
    return value;
}
