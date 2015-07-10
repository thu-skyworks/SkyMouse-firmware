/*
Original Code: https://github.com/Daedaluz/stm32-ws2812/

*/
#include "ws2812.h"
#include "common.h"


/*
任务：修改本代码，实现3个WS2812 LED点亮，并尝试点亮各种颜色
提示：请阅读WS2812的手册，理解修改本代码，点亮另外2个LED。
     由于一个LED需要24位的数据，因此增加的数据为48位。你
     需要修改的代码包含但不限于DMA传输量、数组大小和数组的
     初始化过程。注意发送完所有LED的的数据后等待reset时间
     （即保持输出0一段时间）。
*/

static uint16_t write_buffer[66];

#define CMPH 8
#define CMPL 2
void WS2812_Set() {
    for (int i = 0; i < 8; ++i)
    {
        write_buffer[i] = CMPH;
    }
    for (int i = 8; i < 16; ++i)
    {
        write_buffer[i] = CMPL;
    }
    for (int i = 16; i < 24; ++i)
    {
        write_buffer[i] = CMPH;
    }
}

void WS2812_Init() {

    GPIO_InitTypeDef pwm_gpio = {
        .GPIO_Pin = WS2812_PIN,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Mode = GPIO_Mode_AF_PP
    };

#define CCR_NAME(ch) CCR ## ch
#define CH2CCR(ch) CCR_NAME(ch)
    DMA_InitTypeDef dma = {
        .DMA_PeripheralBaseAddr = (uint32_t)&WS2812_PWM_TIM->CH2CCR(WS2812_TIM_CH),
        .DMA_MemoryBaseAddr = (uint32_t)(&write_buffer[0]),
        .DMA_DIR = DMA_DIR_PeripheralDST,
        .DMA_BufferSize = 66,
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
        .DMA_MemoryInc = DMA_MemoryInc_Enable,
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
        .DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,
        .DMA_Mode = DMA_Mode_Circular,
        .DMA_Priority = DMA_Priority_High,
        .DMA_M2M = DMA_M2M_Disable
    };

    TIM_TimeBaseInitTypeDef tim_init = {
        .TIM_Prescaler = 8,
        .TIM_Period = 9,
        .TIM_ClockDivision = TIM_CKD_DIV1,
        .TIM_CounterMode = TIM_CounterMode_Up
    };
    TIM_OCInitTypeDef pwm;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_TIMClockCmd(WS2812_PWM_TIM, ENABLE);
    RCC_GPIOClockCmd(WS2812_PORT, ENABLE);

    GPIO_Init(WS2812_PORT, &pwm_gpio);
    
    TIM_TimeBaseInit(WS2812_PWM_TIM, &tim_init);

    TIM_OCStructInit(&pwm);
    pwm.TIM_OCMode = TIM_OCMode_PWM1;
    pwm.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCxInit(WS2812_TIM_CH, WS2812_PWM_TIM, &pwm);
    TIM_OCxPreloadConfig(WS2812_TIM_CH, WS2812_PWM_TIM, TIM_OCPreload_Enable);

    RCC_DMAClockCmd(WS2812_DMA, ENABLE);

    DMA_Init(WS2812_DMA_CH, &dma);
    TIM_DMACmd(WS2812_PWM_TIM, WS2812_DMA_SRC, ENABLE);
    
    DMA_Cmd(WS2812_DMA_CH, ENABLE);
    TIM_Cmd(WS2812_PWM_TIM, ENABLE);
}
