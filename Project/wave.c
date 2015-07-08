#include "wave.h"
#include "systick.h"
#include "common.h"

static const uint16_t sin_function[] = { 2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897, 
                                      2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640, 
                                      3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002, 
                                      4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894, 
                                      3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344, 
                                      3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483, 
                                      2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517, 
                                      1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679, 
                                      610, 545, 484, 426, 373, 323, 278, 237, 201, 169, 
                                      141, 118, 100, 87, 79, 75, 76, 82, 93, 109, 
                                      129, 154, 184, 218, 257, 300, 347, 399, 455, 514, 
                                      577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287, 
                                      1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 };  

void WaveGeneration_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;  
    DAC_InitTypeDef DAC_INIT;             

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_GPIOClockCmd(SPEAKER_PORT, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = SPEAKER_PIN;

    GPIO_Init(SPEAKER_PORT, &GPIO_InitStructure);

    DAC_StructInit(&DAC_INIT);
    DAC_INIT.DAC_Trigger = DAC_Trigger_Software;
    DAC_Init(SPEAKER_DAC_CH, &DAC_INIT);

}

void WaveGeneration_Start(void)
{
    DAC_Cmd(SPEAKER_DAC_CH, ENABLE);
    while(true){
        for (int i = 0; i < sizeof(sin_function); ++i)
        {
            DAC_SetChannel1Data(DAC_Align_12b_R, sin_function[i]);
            DAC_SoftwareTriggerCmd(SPEAKER_DAC_CH, ENABLE);
            Delay_us(14);
        }
    }
}
