
#include "common.h"
#include "led.h"
#include "usart.h"
#include "motor.h"
#include "systick.h"
#include "i2c.h"
#include "tmp102.h"
#include "wave.h"
#include "func.h"

static void Init()
{
	SysTick_Init();
	LED_Config();
	USARTx_Config(USART_DBG, 115200);
	Motor_Init();
	I2C_Lib_Init();
}

int main(void)
{

	Init();

	printf("Hello World!\r\n");

	//调用TMP102温度获取函数
	printf("TMP102 temperature: %f\r\n", TMP102_GetTemp());

	//调用信号发生器程序
	WaveGeneration_Init();
	WaveGeneration_Start();

	while(true) {
	}
}
