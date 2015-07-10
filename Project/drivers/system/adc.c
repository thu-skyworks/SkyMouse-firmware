#include "adc.h"
#include "common.h"

/*
 * ADC1 初始化，指定通道数。注意，初始化之后必须分别配置每个通道。
 */
void ADC_Config(uint8_t nChannel)
{
    ADC_InitTypeDef  ADC_InitStructure;
    ADC_StructInit(&ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_CFGR_ADCPRE_DIV6);//ADC时钟PLCK2的6分频 12M。ADC最大时钟不能超过14M!
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_Cmd(ADC1,DISABLE);

    if(nChannel == 0)
        return;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; //使能扫描
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件控制转换
    ADC_InitStructure.ADC_NbrOfChannel = nChannel;

    ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC
}

void ADC_Channel_Config(GPIO_TypeDef* GPIOx, uint16_t pin, uint8_t channel, uint8_t rank)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_GPIOClockCmd(GPIOx, ENABLE);  //GPIO时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = pin;

    GPIO_Init(GPIOx, &GPIO_InitStructure);

    DBG_MSG("Regular %d is channel %d", rank, channel);
    ADC_RegularChannelConfig(ADC1, channel, rank, ADC_SampleTime_13Cycles5);
}

void ADC_DMA_Init(volatile uint16_t *ADCConvertedValues, uint32_t size)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    
    /* YOUR CODE HERE 
    任务：设置DMA_InitStructure的各个成员，用于初始化DMA，使其将ADC1采样到的值
         自动地存储到ADCConvertedValues数组中，存储值的数量由size参数指定。
    提示：DMA的源地址应当为ADC1的DR寄存器（该寄存器中保存着每次转换的值，且每次转换
         会覆盖上一次的值），目的地址是ADCConvertedValues，传输长度为size。请思考
         源和目标的地址自增功能分别如何设置。另外，为了配合ADC自动循环采样，DMA模式
         为循环模式。
    相关函数：
        STM32固件库函数手册的DMA_Init函数的描述中，包含了DMA_InitTypeDef结构体各
        成员的设置方法。
    */

    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
}

void ADC_Prepare()
{
    ADC_Cmd(ADC1, ENABLE);//使能ADC1
    
    ADC_ResetCalibration(ADC1); //重置ADC1校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC1校准重置完成
    ADC_StartCalibration(ADC1);//开始ADC1校准
    while(ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
}

void ADC_StartSample()
{
    ADC_Cmd(ADC1, ENABLE);//使能ADC1
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ADC1软件转换
}
