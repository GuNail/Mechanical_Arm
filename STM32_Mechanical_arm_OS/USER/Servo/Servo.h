#ifndef __SERVO_H_
#define __SERVO_H_
#include "main.h"

struct servo_data_def
{
	uint16_t now_angle[6];
	uint16_t target_angle[6];

};

void Servo_Init(void);

void Servo_Base_Control_0(uint16_t angle);
void Servo_Base_Control_1(uint16_t angle);
void Servo_Base_Control_2(uint16_t angle);
void Servo_Base_Control_3(uint16_t angle);
void Servo_Base_Control_4(uint16_t angle);
void Servo_Base_Control_5(uint16_t angle);

inline void Servo_Control_All(uint16_t angle0,uint16_t angle1,uint16_t angle2,uint16_t angle3,uint16_t angle4,uint16_t angle5);
void Servo_SingleControl(uint8_t num, uint16_t angle);
void Servo_Control(struct servo_data_def servo_data);
uint16_t Servo_Getdata(uint8_t num);

uint16_t convertToPulseTime(float angle);
uint16_t convertToAngle(uint16_t pulseTime);

uint16_t convertToPulseTime180(float angle);
uint16_t convertToAngle180(uint16_t pulseTime);
#endif

