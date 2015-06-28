#include "motor.h"
#include "common.h"

enum{DRV_BRAKE, DRV_DIR1, DRV_DIR2};

static void Motor_GPIO_Config(GPIO_TypeDef *port, uint16_t pin1, uint16_t pin2, uint8_t type)
{
    GPIO_InitTypeDef gpio_init = {
        .GPIO_Speed = GPIO_Speed_50MHz,
    };
    if(type == DRV_BRAKE){
        gpio_init.GPIO_Pin = pin1|pin2;
        gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(port, &gpio_init);
        GPIO_SetBits(port, pin1|pin2);

        // GPIO_SetBits(port, pin1);
        // GPIO_ResetBits(port, pin2);
    }else{
        gpio_init.GPIO_Pin = type==DRV_DIR1 ? pin1 : pin2;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(port, &gpio_init);

        gpio_init.GPIO_Pin = type==DRV_DIR2 ? pin1 : pin2;
        gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(port, &gpio_init);

        GPIO_ResetBits(port, pin1|pin2);
    }
}

void Motor_Init()
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_TIMClockCmd(MOTORA_PWM_TIM, ENABLE);
    RCC_TIMClockCmd(MOTORB_PWM_TIM, ENABLE);

    RCC_GPIOClockCmd(MOTORA_PORT, ENABLE);
    RCC_GPIOClockCmd(MOTORB_PORT, ENABLE);

    Motor_GPIO_Config(MOTORA_PORT, MOTORA1_PIN, MOTORA2_PIN, DRV_BRAKE);
    Motor_GPIO_Config(MOTORB_PORT, MOTORB1_PIN, MOTORB2_PIN, DRV_BRAKE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    /* Time base configuration */        
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 15;

    TIM_TimeBaseInit(MOTORA_PWM_TIM, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(MOTORB_PWM_TIM, &TIM_TimeBaseStructure);

    // TIM_ARRPreloadConfig(MOTORA_PWM_TIM, ENABLE);
    // TIM_ARRPreloadConfig(MOTORB_PWM_TIM, ENABLE);

    TIM_SetCounter(MOTORA_PWM_TIM, 0);
    TIM_SetCounter(MOTORB_PWM_TIM, 0);

    TIM_Cmd(MOTORA_PWM_TIM, ENABLE);
    TIM_Cmd(MOTORB_PWM_TIM, ENABLE);

    TIM_CtrlPWMOutputs(MOTORA_PWM_TIM, ENABLE);
    TIM_CtrlPWMOutputs(MOTORB_PWM_TIM, ENABLE);
}

void Motor_SetSpeed(uint8_t motor, uint16_t speed, bool dir_rev)
{
    if(motor & MOTOR_L){
        TIMx_OCx_Config(MOTORB_PWM_TIM, dir_rev ? MOTORB1_CH : MOTORB2_CH, speed, true);
        Motor_GPIO_Config(MOTORB_PORT, MOTORB1_PIN, MOTORB2_PIN, dir_rev ? DRV_DIR1 : DRV_DIR2);
    }
    if(motor & MOTOR_R){
        TIMx_OCx_Config(MOTORA_PWM_TIM, dir_rev ? MOTORA1_CH : MOTORA2_CH, speed, true);
        Motor_GPIO_Config(MOTORA_PORT, MOTORA1_PIN, MOTORA2_PIN, dir_rev ? DRV_DIR1 : DRV_DIR2);
    }
}

void Motor_Brake(uint8_t motor)
{
    if(motor & MOTOR_L){
        Motor_GPIO_Config(MOTORB_PORT, MOTORB1_PIN, MOTORB2_PIN, DRV_BRAKE);
    }
    if(motor & MOTOR_R){
        Motor_GPIO_Config(MOTORA_PORT, MOTORA1_PIN, MOTORA2_PIN, DRV_BRAKE);
    }
}
