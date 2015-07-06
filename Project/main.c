
#include "common.h"
#include "led.h"
#include "usart.h"
#include "motor.h"
#include "systick.h"
#include "i2c.h"
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


	while(true) {
		/* YOUR CODE HERE 
		任务：使得小老鼠正面的全彩LED，按照红绿蓝顺序依次点亮
		提示：在led.c文件中，已经实现了GPIO的初始化和控制的代码。
			请参考其中的代码实现，或者直接调用。LED引脚关系如下：
			Red->PB12, Green->PB1, Blue->PB15
		辅助函数：
			void Delay_ms(unsigned int n);
			产生n毫秒的延时，即使得代码停止运行n毫秒。以便实现闪烁的效果。
		*/
	}
}
