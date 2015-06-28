
#include "common.h"
#include "led.h"
#include "usart.h"
#include "motor.h"
#include "systick.h"
#include "usbcommon.h"
#include "usb_lib.h"
#include "sdio.h"
#include "ff.h"
#include "esp8266.h"
#include "ws2812.h"
#include "mpu9250.h"
#include "tmp102.h"
#include "i2c.h"
#include "func.h"

static void Init()
{
	SysTick_Init();
	LED_Config();
	USARTx_Config(USART_DBG, 115200);
	Motor_Init();
	USBCommon_Init();
	WS2812_Init();
	I2C_Lib_Init();
}

static void FileSystem_Init(void)
{
	static FATFS fileSystem;
	SD_Error Status = SD_OK;
	SD_CardInfo SDCardInfo;

	for(int i=0;i<3;i++)
	{
		if((Status = SD_Init()) != SD_OK){
			DBG_MSG("SD Card Init Failed! Retrying...", 0);
		}
		else
		{
			break;
		}
	}
	if(Status != SD_OK) {
		ERR_MSG("No SD card found!", 0);
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
		ERR_MSG("Failed to mount SD card!", 0);
		return;
	}

	DBG_MSG("SD Card Init OK!", 0);
}

static void fileTest()
{
	static FIL fileObj; //must be static
	char buf[16];
	uint32_t cnt;
	const char* file = "hello.txt";

	if(f_open(&fileObj, file, FA_OPEN_EXISTING|FA_READ) != FR_OK){
		ERR_MSG("Failed to open %s!", file);
		f_mount(0, 0);
		return;
	}

	if(FR_OK != f_read(&fileObj, buf, sizeof(buf), &cnt)){
		ERR_MSG("f_read failed!");
	}else{
		buf[cnt] = 0;
		DBG_MSG("Content: [%s]", buf);
	}


	f_close(&fileObj);

}

static void UpgradeMode_ESP8266(void)
{
	if(USBDevice_PlugIn())
	{
		Delay_ms(2000);
		ESP8266_Enable_CDC_Forwarding();
		DBG_MSG( "Usb Init Started");
		USB_Init();
		DBG_MSG( "Usb Init Succeeded");
	}

	DBG_MSG("ESP8266 Upgrade Mode...");
	while(true){
		LED_BLUE(true);
		Delay_ms(200);
		LED_BLUE(false);
		Delay_ms(200);
	}
}

int main(void)
{
	bool upgradeMode = ESP8266_UpgradeModeDetected();

	Init();

	DBG_MSG("----- Power On -----");
	ESP8266_Init(upgradeMode);

	if(upgradeMode)
		UpgradeMode_ESP8266();

	// char color[] = {0xff, 0xff, 0x00};
	// WS2812_Set(0, 3, color);
	// color[0] = 0x00;
	// color[2] = 0xff;
	// WS2812_Set(3, 3, color);
	// color[0] = 0xff;
	// color[1] = 0x00;
	// WS2812_Set(6, 3, color);

	FileSystem_Init();
	fileTest();
	// WavePlayer_Init();
	// Motor_SetSpeed(MOTOR_L|MOTOR_R, 1500, false);

	// DBG_MSG("Who Am I: %d", MPU9250_readWhoAmI());
	// Delay_ms(100);
	// DBG_MSG("Who Am I: %d", MPU9250_readWhoAmI());

	// LED_GREEN(true);
	// Delay_ms(1000);
	// LED_BLUE(true);
	// Delay_ms(1000);
	// LED_RED(true);

	while(true) {
		Delay_ms(1000);
		DBG_MSG("Temp: %f", TMP102_GetTemp());
	}
}

#ifdef  USE_FULL_ASSERT

/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}

#endif
