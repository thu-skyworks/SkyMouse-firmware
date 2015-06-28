/*
Original Code: https://github.com/Daedaluz/stm32-ws2812/

*/
#include "ws2812.h"
#include "common.h"

#define BUFFER_SIZE (3*24 + 42)

static uint16_t write_buffer[BUFFER_SIZE];

//#define CMPH 17
//#define CMPL 9

#define CMPH 8
#define CMPL 2
void WS2812_Set(uint32_t offset, uint32_t size, char* data) {
    uint32_t x = 0;
    for(x = 0; x < size; x++) {
        write_buffer[((x+offset)*8)+0] = (data[x] & 0x80) ? CMPH:CMPL;
        write_buffer[((x+offset)*8)+1] = (data[x] & 0x40) ? CMPH:CMPL;
        write_buffer[((x+offset)*8)+2] = (data[x] & 0x20) ? CMPH:CMPL;
        write_buffer[((x+offset)*8)+3] = (data[x] & 0x10) ? CMPH:CMPL;
        write_buffer[((x+offset)*8)+4] = (data[x] & 0x08) ? CMPH:CMPL;
        write_buffer[((x+offset)*8)+5] = (data[x] & 0x04) ? CMPH:CMPL;
        write_buffer[((x+offset)*8)+6] = (data[x] & 0x02) ? CMPH:CMPL;
        write_buffer[((x+offset)*8)+7] = (data[x] & 0x01) ? CMPH:CMPL;
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
        .DMA_BufferSize = BUFFER_SIZE,
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
        .DMA_MemoryInc = DMA_MemoryInc_Enable,
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
        .DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,
        .DMA_Mode = DMA_Mode_Circular,
        .DMA_Priority = DMA_Priority_High,
        .DMA_M2M = DMA_M2M_Disable
    };

    TIM_TimeBaseInitTypeDef tim_init = {
    //  .TIM_Prescaler = 2,
        .TIM_Prescaler = 8,
    //  .TIM_Period = 29,
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
