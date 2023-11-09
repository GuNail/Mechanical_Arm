/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UART.h"
#include "app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart1;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for Servo_Task */
osThreadId_t Servo_TaskHandle;
const osThreadAttr_t Servo_Task_attributes = {
    .name = "Servo_Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityHigh7,
};
/* Definitions for USART1_RX_Task */
osThreadId_t USART1_RX_TaskHandle;
const osThreadAttr_t USART1_RX_Task_attributes = {
    .name = "USART1_RX_Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityRealtime6,
};
/* Definitions for Servo_data_uplo */
osThreadId_t Servo_data_uploHandle;
const osThreadAttr_t Servo_data_uplo_attributes = {
    .name = "Servo_data_uplo",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for servoRotateTask */
osThreadId_t servoRotateTaskHandle;
const osThreadAttr_t servoRotateTask_attributes = {
    .name = "servoRotateTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityRealtime,
};
/* Definitions for Servo_Tx_Queue */
osMessageQueueId_t Servo_Tx_QueueHandle;
const osMessageQueueAttr_t Servo_Tx_Queue_attributes = {.name = "Servo_Tx_Queue"};
/* Definitions for LED_Timer0 */
osTimerId_t LED_Timer0Handle;
const osTimerAttr_t LED_Timer0_attributes = {.name = "LED_Timer0"};
/* Definitions for UART_RX_Finsh */
osSemaphoreId_t UART_RX_FinshHandle;
const osSemaphoreAttr_t UART_RX_Finsh_attributes = {.name = "UART_RX_Finsh"};
/* Definitions for Servo_Control */
osSemaphoreId_t Servo_ControlHandle;
const osSemaphoreAttr_t Servo_Control_attributes = {.name = "Servo_Control"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Servo_Task_Fun(void *argument);
void USART1_RX_Task_Fun(void *argument);
void Servo_data_upload_Fun(void *argument);
void servoRotateTask_Fun(void *argument);
void LED_Timer0_Callback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* Create the semaphores(s) */
    /* creation of UART_RX_Finsh */
    UART_RX_FinshHandle = osSemaphoreNew(1, 0, &UART_RX_Finsh_attributes);

    /* creation of Servo_Control */
    Servo_ControlHandle = osSemaphoreNew(1, 0, &Servo_Control_attributes);

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* Create the timer(s) */
    /* creation of LED_Timer0 */
    LED_Timer0Handle = osTimerNew(LED_Timer0_Callback, osTimerPeriodic, NULL, &LED_Timer0_attributes);

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the queue(s) */
    /* creation of Servo_Tx_Queue */
    Servo_Tx_QueueHandle = osMessageQueueNew(10, 4, &Servo_Tx_Queue_attributes);

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of Servo_Task */
    Servo_TaskHandle = osThreadNew(Servo_Task_Fun, NULL, &Servo_Task_attributes);

    /* creation of USART1_RX_Task */
    USART1_RX_TaskHandle = osThreadNew(USART1_RX_Task_Fun, NULL, &USART1_RX_Task_attributes);

    /* creation of Servo_data_uplo */
    Servo_data_uploHandle = osThreadNew(Servo_data_upload_Fun, NULL, &Servo_data_uplo_attributes);

    /* creation of servoRotateTask */
    servoRotateTaskHandle = osThreadNew(servoRotateTask_Fun, NULL, &servoRotateTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */

    osTimerStart(LED_Timer0Handle, 500);
    for (;;)
    {
        osDelay(1);
    }

    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Servo_Task_Fun */
/**
 * @brief Function implementing the Servo_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Servo_Task_Fun */
__weak void Servo_Task_Fun(void *argument)
{
    /* USER CODE BEGIN Servo_Task_Fun */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END Servo_Task_Fun */
}

/* USER CODE BEGIN Header_USART1_RX_Task_Fun */
/**
 * @brief Function implementing the USART1_RX_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_USART1_RX_Task_Fun */
__weak void USART1_RX_Task_Fun(void *argument)
{
    /* USER CODE BEGIN USART1_RX_Task_Fun */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END USART1_RX_Task_Fun */
}

/* USER CODE BEGIN Header_Servo_data_upload_Fun */
/**
 * @brief Function implementing the Servo_data_uplo thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Servo_data_upload_Fun */
__weak void Servo_data_upload_Fun(void *argument)
{
    /* USER CODE BEGIN Servo_data_upload_Fun */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END Servo_data_upload_Fun */
}

/* USER CODE BEGIN Header_servoRotateTask_Fun */
/**
 * @brief Function implementing the servoRotateTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_servoRotateTask_Fun */
__weak void servoRotateTask_Fun(void *argument)
{
    /* USER CODE BEGIN servoRotateTask_Fun */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END servoRotateTask_Fun */
}

/* LED_Timer0_Callback function */
void LED_Timer0_Callback(void *argument)
{
    /* USER CODE BEGIN LED_Timer0_Callback */
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    /* USER CODE END LED_Timer0_Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
