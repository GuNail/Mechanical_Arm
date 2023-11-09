#include "app.h"
#include "Servo.h"
#include "UART.h"
#include "cmsis_os.h"
#include "usart.h"

struct servo_data_def servo_data;
extern osSemaphoreId_t Servo_ControlHandle;
extern osMessageQueueId_t Servo_Tx_QueueHandle;

uint8_t finish[] = {0x55, 0x0B, 0x01, 0x00, 0x00, 0x00, 0x1D, 0xBB};

void Servo_Task_Fun(void *argument)
{
	osStatus_t restat;
	uint8_t rx_data[4] = {0};
	uint16_t angle = 0;

	while (1)
	{
		restat = osMessageQueueGet(Servo_Tx_QueueHandle, rx_data, 0, osWaitForever);

		if (restat == osOK)
		{

			if (rx_data[0] == 0x00) // �����������ĽǶȣ���������
			{
				angle = ((rx_data[2] << 8) | rx_data[3]);
				servo_data.target_angle[rx_data[1]] = angle;
			}
			else if (rx_data[0] == 0x01) // �������ĽǶȣ�������
			{
				angle = ((rx_data[2] << 8) | rx_data[3]);
				Servo_SingleControl(rx_data[1], angle);
				upload(finish);
			}
			else if (rx_data[0] == 0x10) // �������
			{
				osSemaphoreRelease(Servo_ControlHandle);
			}
		}
		// osDelay(10);
	}
}

void USART1_RX_Task_Fun(void *argument)
{
	extern osSemaphoreId_t UART_RX_FinshHandle;
	extern struct FIFO_Typedef UART1_FIFO;
	extern struct FIFO_Typedef UART3_FIFO;
	extern uint8_t UART_Number;

	uint8_t packNum = 0;
	uint8_t buffer[4] = {0};

	osStatus_t Ret = osOK;

	while (1)
	{
		UART1_Start_Receive();
		UART3_Start_Receive();
		Ret = osSemaphoreAcquire(UART_RX_FinshHandle, osWaitForever);

		if (Ret == osOK)
		{
			if (UART_Number == 1)
			{
				if ((UART1_FIFO.head % 8) == 0)
				{
					packNum = (UART1_FIFO.head / 8);

					for (uint8_t i = 0; i < packNum; i++)
					{
						if (UART1_FIFO.buf[0 + 8 * i] == 0x55 && UART1_FIFO.buf[1 + 8 * i] == 0x0A &&
						        UART1_FIFO.buf[7 + 8 * i] == 0xBB)
						{
							if (cheak_sum(UART1_FIFO.buf, i) == UART1_FIFO.buf[6 + 8 * i])
							{
								buffer[0] = UART1_FIFO.buf[2 + 8 * i];
								buffer[1] = UART1_FIFO.buf[3 + 8 * i];
								buffer[2] = UART1_FIFO.buf[4 + 8 * i];
								buffer[3] = UART1_FIFO.buf[5 + 8 * i];

								osMessageQueuePut(Servo_Tx_QueueHandle, buffer, 0, 0);
							}
						}
					}
				}
			}
			else if (UART_Number == 3)
			{
				if ((UART3_FIFO.head % 8) == 0)
				{
					packNum = (UART3_FIFO.head / 8);

					for (uint8_t i = 0; i < packNum; i++)
					{
						if (UART3_FIFO.buf[0 + 8 * i] == 0x55 && UART3_FIFO.buf[1 + 8 * i] == 0x0A &&
						        UART3_FIFO.buf[7 + 8 * i] == 0xBB)
						{
							if (cheak_sum(UART3_FIFO.buf, i) == UART3_FIFO.buf[6 + 8 * i])
							{
								buffer[0] = UART3_FIFO.buf[2 + 8 * i];
								buffer[1] = UART3_FIFO.buf[3 + 8 * i];
								buffer[2] = UART3_FIFO.buf[4 + 8 * i];
								buffer[3] = UART3_FIFO.buf[5 + 8 * i];

								osMessageQueuePut(Servo_Tx_QueueHandle, buffer, 0, 0);
							}
						}
					}
				}
			}
		}

		// osDelay(10);
	}
}

void servoRotateTask_Fun(void *argument)
{
	osStatus_t Ret = osOK;

	int step[4];
	int i;

	while (1)
	{
		Ret = osSemaphoreAcquire(Servo_ControlHandle, osWaitForever);

		if (Ret == osOK)
		{

			for (i = 0; i < 4; i++)
			{
				step[i] = (servo_data.target_angle[i] > servo_data.now_angle[i]) ? 1 : -1;
				//				while(servo_data.target_angle[i]!=servo_data.now_angle[i])
				//				{
				//					servo_data.now_angle[i]+=step;
				//					Servo_Control(servo_data);
				//					osDelay(5);
				//				}
			}

			while ( servo_data.target_angle[0] != servo_data.now_angle[0] ||
			        servo_data.target_angle[1] != servo_data.now_angle[1] ||
			        servo_data.target_angle[2] != servo_data.now_angle[2] ||
			        servo_data.target_angle[3] != servo_data.now_angle[3])
			{
				if (servo_data.target_angle[0] != servo_data.now_angle[0])
				{
					servo_data.now_angle[0] += step[0];
				}
				
				if (servo_data.target_angle[1] != servo_data.now_angle[1])
				{
					servo_data.now_angle[1] += step[1];
				}

				if (servo_data.target_angle[2] != servo_data.now_angle[2])
				{
					servo_data.now_angle[2] += step[2];
				}

				if (servo_data.target_angle[3] != servo_data.now_angle[3])
				{
					servo_data.now_angle[3] += step[3];
				}
				
//				if (servo_data.target_angle[4] != servo_data.now_angle[4])
//				{
//					servo_data.now_angle[4] += step[4];
//				}
//				
//				if (servo_data.target_angle[5] != servo_data.now_angle[5])
//				{
//					servo_data.now_angle[5] += step[5];
//				}

				Servo_Control(servo_data);
				osDelay(10);
			}
		}

		upload(finish);
	}
}

void Servo_data_upload_Fun(void *argument)
{
	// ֡ͷ  ��/�ӻ�	 ģʽѡ��  ������  ��λ����   ��λ����  cheak 	֡β
	uint8_t servo_updata[8] = {0x55, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};
	uint8_t num = 0;

	while (1)
	{
		servo_data.now_angle[num] = Servo_Getdata(num);

		servo_updata[3] = num;
		servo_updata[4] = (servo_data.now_angle[num] & 0xFF00) >> 8;
		servo_updata[5] = servo_data.now_angle[num] & 0x00FF;
		servo_updata[6] = cheak_sum(servo_updata, 0);

		num++;
		num %= 6;

		upload(servo_updata);
		osDelay(150);
	}
}
