#ifndef MOTOR_H__
#define MOTOR_H__

#include "common.h"

enum
{
    MOTOR_L = 1<<0,
    MOTOR_R = 1<<1
};

void Motor_Init(void);
void Motor_SetSpeed(uint8_t motor, uint16_t speed, bool dir_rev);
void Motor_Brake(uint8_t motor);

#endif