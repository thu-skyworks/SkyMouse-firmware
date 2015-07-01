#include "common.h"
#include "analog.h"
#include "motor.h"
#include "led.h"

void Reflective_Start(void)
{
    bool dir = true;
    const int MAX_SPEED = 30000;
    Analog_SetChannel(REFLECTIVE_L_CH, true);
    Analog_SetChannel(REFLECTIVE_R_CH, true);
    // Motor_SetSpeed(MOTOR_L|MOTOR_R, MAX_SPEED, dir);
    while(true){
        uint16_t L, R;
        L = Analog_GetChannelValue(REFLECTIVE_L_CH);
        R = Analog_GetChannelValue(REFLECTIVE_R_CH);
        // DBG_MSG("L:%d R:%d", L, R);
        if(L >= 1000){
            LED_RED(false);
            Motor_Brake(MOTOR_L);
            // Motor_SetSpeed(MOTOR_L, 100, dir);
            Delay_ms(500);
        }else{
            LED_RED(true);
            Motor_SetSpeed(MOTOR_L, MAX_SPEED, dir);
        }
        if(R >= 1000){
            LED_BLUE(false);
            Motor_Brake(MOTOR_R);
            // Motor_SetSpeed(MOTOR_R, 100, dir);
            Delay_ms(500);
        }else{
            LED_BLUE(true);
            Motor_SetSpeed(MOTOR_R, MAX_SPEED, dir);
        }
    }
}