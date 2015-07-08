
#include "common.h"
#include "led.h"
#include "usart.h"
#include "motor.h"
#include "systick.h"
#include "i2c.h"
#include "tmp102.h"
#include "wave.h"
#include "waveplayer.h"
#include "ff.h"
#include "sdio.h"
#include "func.h"

static void Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SysTick_Init();
	LED_Config();
	USARTx_Config(USART_DBG, 115200);
	Motor_Init();
	I2C_Lib_Init();
}

static void FileSystem_Init(void)
{
	static FATFS fileSystem;
	SD_Error Status = SD_OK;
	SD_CardInfo SDCardInfo;

	Delay_ms(1000);
	for(int i=0;i<3;i++)
	{
		if((Status = SD_Init()) != SD_OK){
			DBG_MSG("SD Card Init Failed! Retrying...");
		}
		else
		{
			break;
		}
	}
	if(Status != SD_OK) {
		ERR_MSG("No SD card found!");
		return;
	}

	Status = SD_GetCardInfo( &SDCardInfo );
    if(Status != SD_OK)
        return;

    DBG_MSG("CardCapacity: %dM, Block Size %d",
    	(int)(SDCardInfo.CardCapacity/1024/1024),
    	(int)SDCardInfo.CardBlockSize);
    DBG_MSG("CardType: %d", (int)SDCardInfo.CardType);

	if(f_mount(0, &fileSystem) != FR_OK){
		ERR_MSG("Failed to mount SD card!");
		return;
	}

	DBG_MSG("SD Card Init OK!");
}

int main(void)
{

	Init();
	Delay_ms(100);

	printf("Hello World!\r\n");

	//调用TMP102温度获取函数
	printf("TMP102 temperature: %f\r\n", TMP102_GetTemp());

	//调用信号发生器程序
	WaveGeneration_Init();
	WaveGeneration_Start();

	//音乐播放器demo，若要使用，请注释掉上面WaveGeneration的两个函数
	Delay_ms(2000);
	FileSystem_Init();
	WavePlayer_Init();
	WavePlayerMenu_Start("/", "test.wav");

	while(true) {
	}
}
