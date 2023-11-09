#include "Servo.h"
#include "tim.h"

extern struct servo_data_def servo_data;

void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

    servo_data.target_angle[0] = 90;
    servo_data.target_angle[1] = 90;
    servo_data.target_angle[2] = 90;
    servo_data.target_angle[3] = 90;
    servo_data.target_angle[4] = 90;
    servo_data.target_angle[5] = 90;

    Servo_Control_All(90, 90, 90, 90, 90, 90);
}

inline void Servo_Base_Control_0(uint16_t angle)
{
    uint32_t num = convertToPulseTime(angle);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, num);
}

inline void Servo_Base_Control_1(uint16_t angle)
{
    uint32_t num = convertToPulseTime(angle);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, num);
}

inline void Servo_Base_Control_2(uint16_t angle)
{
    uint32_t num = convertToPulseTime(angle);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, num);
}

inline void Servo_Base_Control_3(uint16_t angle)
{
    uint32_t num = convertToPulseTime(angle);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, num);
}

inline void Servo_Base_Control_4(uint16_t angle)
{
    uint32_t num = convertToPulseTime(angle);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, num);
}

inline void Servo_Base_Control_5(uint16_t angle)
{
    uint32_t num = convertToPulseTime180(angle);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, num);
}

inline void Servo_Control_All(uint16_t angle0, uint16_t angle1, uint16_t angle2, uint16_t angle3, uint16_t angle4,
                              uint16_t angle5)
{
    Servo_Base_Control_0(angle0);
    Servo_Base_Control_1(angle1);
    Servo_Base_Control_2(angle2);
    Servo_Base_Control_3(angle3);
    Servo_Base_Control_4(angle4);
    Servo_Base_Control_5(angle5);
}

void Servo_Control(struct servo_data_def servo_data)
{
    Servo_Base_Control_0(servo_data.now_angle[0]);
    Servo_Base_Control_1(servo_data.now_angle[1]);
    Servo_Base_Control_2(servo_data.now_angle[2]);
    Servo_Base_Control_3(servo_data.now_angle[3]);
//    Servo_Base_Control_4(servo_data.now_angle[4]);
//    Servo_Base_Control_5(servo_data.now_angle[5]);
}

void Servo_SingleControl(uint8_t num, uint16_t angle)
{
    switch (num)
    {
        case 0:
            Servo_Base_Control_0(angle);
            break;

        case 1:
            Servo_Base_Control_1(angle);
            break;

        case 2:
            Servo_Base_Control_2(angle);
            break;

        case 3:
            Servo_Base_Control_3(angle);
            break;

        case 4:
            Servo_Base_Control_4(angle);
            break;

        case 5:
            Servo_Base_Control_5(angle);
            break;
    }
}

uint16_t Servo_Getdata(uint8_t num)
{
    uint16_t angle = 0;

    switch (num)
    {
        case 0:
            angle = convertToAngle(__HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_2) + 1);
            break;

        case 1:
            angle = convertToAngle(__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_3) + 1);
            break;

        case 2:
            angle = convertToAngle(__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_4) + 1);
            break;

        case 3:
            angle = convertToAngle(__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1) + 1);
            break;

        case 4:
            angle = convertToAngle(__HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_2) + 1);
            break;

        case 5:

            angle = convertToAngle180(__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) + 1);
            break;
    }

    return angle;
}

// 将舵机角度转换为对应的脉冲时间（微秒）
uint16_t convertToPulseTime(float angle)
{
    if (angle < 0)
    {
        angle = 0;
    }
    else if (angle > 180)
    {
        angle = 180;
    }
    uint16_t pulseTime = 833 + (angle / 270.0 * 2000.0);
    return pulseTime;
}

uint16_t convertToAngle(uint16_t pulseTime)
{
    uint16_t angle = (pulseTime - 833) / 2000.0 * 270.0;
    return angle;
}

uint16_t convertToPulseTime180(float angle)
{
    uint16_t pulseTime = 500 + (angle / 180.0 * 2000.0);
    return pulseTime;
}

uint16_t convertToAngle180(uint16_t pulseTime)
{
    uint16_t angle = (pulseTime - 500) / 2000.0 * 180.0;
    return angle;
}
