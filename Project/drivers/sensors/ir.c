#include "common.h"
#include "ir.h"
#include "adc.h"
#include "systick.h"


static void getIOByChannel(uint8_t channel, GPIO_TypeDef** GPIOx, uint16_t* pin)
{
    switch(channel){
        case 0:
            *pin = GPIO_Pin_0;
            break;
        case 1:
            *pin = GPIO_Pin_1;
            break;
        case 2:
            *pin = GPIO_Pin_2;
            break;
        case 3:
            *pin = GPIO_Pin_3;
            break;
        case 4:
            *pin = GPIO_Pin_4;
            break;
        case 5:
            *pin = GPIO_Pin_5;
            break;
        case 6:
            *pin = GPIO_Pin_6;
            break;
        case 7:
            *pin = GPIO_Pin_7;
            break;
        case 8:
            *pin = GPIO_Pin_0;
            break;
        case 9:
            *pin = GPIO_Pin_1;
            break;
        case 10:
            *pin = GPIO_Pin_0;
            break;
        case 11:
            *pin = GPIO_Pin_1;
            break;
        case 12:
            *pin = GPIO_Pin_2;
            break;
        case 13:
            *pin = GPIO_Pin_3;
            break;
        case 14:
            *pin = GPIO_Pin_4;
            break;
        case 15:
            *pin = GPIO_Pin_5;
            break;
    }
    if(channel <= 7)
        *GPIOx = GPIOA;
    else if(8<=channel && channel<=9)
        *GPIOx = GPIOB;
    else if(10<=channel && channel<=15)
        *GPIOx = GPIOC;
}

void IR_SampleSensors(void)
{
    uint8_t channels[7] = {
        REFLECTIVE_L_CH, REFLECTIVE_R_CH,
        PHOTOTRANS_1_CH, PHOTOTRANS_2_CH, 
        PHOTOTRANS_3_CH, PHOTOTRANS_4_CH, 
        PHOTOTRANS_5_CH
    };
    GPIO_TypeDef* gpio;
    uint16_t pin;
    uint16_t results[7] = {0};
    ADC_Config(7);

    for (int i = 0; i < 7; ++i)
    {
        getIOByChannel(channels[i], &gpio, &pin);
        ADC_Channel_Config(gpio, pin, channels[i], i+1);
    }

    ADC_DMA_Init(results, 7);

    ADC_Prepare();

    ADC_StartSample();

    while(true){
        Delay_ms(1000);
        for (int i = 0; i < 7; ++i)
        {
            printf("results[%d]=%d\r\n", i, results[i]);
        }
        printf("----------------\r\n");
    }

}

