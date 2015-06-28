#ifndef PIN_H__
#define PIN_H__

//I2C1_DevStructure is defined in CPAL
//All sensors with i2c interface is connected to I2C1
#define I2C_HOST_DEV I2C1_DevStructure

#define MPU9250_INT_PORT GPIOB
#define MPU9250_INT_PIN  GPIO_Pin_9

#define WS2812_PORT     GPIOB
#define WS2812_PIN      GPIO_Pin_8
#define WS2812_PWM_TIM  TIM4
#define WS2812_TIM_CH   3
#define WS2812_DMA      DMA1
#define WS2812_DMA_CH   DMA1_Channel5
#define WS2812_DMA_SRC  TIM_DMA_CC3

#define MOTORA_PORT   GPIOB
#define MOTORA1_PIN   GPIO_Pin_14
#define MOTORA2_PIN   GPIO_Pin_13
#define MOTORA_PWM_TIM    TIM1
#define MOTORA1_CH       2
#define MOTORA2_CH       1

#define MOTORB_PORT   GPIOC
#define MOTORB1_PIN   GPIO_Pin_6
#define MOTORB2_PIN   GPIO_Pin_7
#define MOTORB_PWM_TIM    TIM8
#define MOTORB1_CH       1
#define MOTORB2_CH       2

#define SPEAKER_DAC_CH  DAC_Channel_1
#define SPEAKER_PORT    GPIOA
#define SPEAKER_PIN     GPIO_Pin_4

#define LED_BRD_PORT  GPIOB
#define LED_BRD_R_PIN  GPIO_Pin_12
#define LED_BRD_B_PIN  GPIO_Pin_15
#define LED_BRD_G_PIN  GPIO_Pin_1

#define USART_DBG     USART1

#define ESP8266_IO_PORT GPIOC
#define ESP8266_IO0_PIN GPIO_Pin_15
#define ESP8266_USART   USART2
#define ESP8266_USART_IRQ   USART2_IRQn

#define USB_DET_PORT GPIOC
#define USB_DET_PIN  GPIO_Pin_0

#define SD_DETECT_GPIO_CLK  RCC_APB2Periph_GPIOC
#define SD_DETECT_GPIO_PORT GPIOC
#define SD_DETECT_PIN       GPIO_Pin_13

#endif
