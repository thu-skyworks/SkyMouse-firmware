/**
  ******************************************************************************
  * @file    waveplayer.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    08-April-2011
  * @brief   This file contains all the functions prototypes for the wave player
  *          firmware driver.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WAVEPLAYER_H
#define __WAVEPLAYER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
@code 
  .WAV file format :
 Endian      Offset      Length      Contents
  big         0           4 bytes     'RIFF'             // 0x52494646
  little      4           4 bytes     <file length - 8>
  big         8           4 bytes     'WAVE'             // 0x57415645
Next, the fmt chunk describes the sample format:
  big         12          4 bytes     'fmt '          // 0x666D7420
  little      16          4 bytes     0x00000010      // Length of the fmt data (16 bytes)
  little      20          2 bytes     0x0001          // Format tag: 1 = PCM
  little      22          2 bytes     <channels>      // Channels: 1 = mono, 2 = stereo
  little      24          4 bytes     <sample rate>   // Samples per second: e.g., 22050
  little      28          4 bytes     <bytes/second>  // sample rate * block align
  little      32          2 bytes     <block align>   // channels * bits/sample / 8
  little      34          2 bytes     <bits/sample>   // 8 or 16
Finally, the data chunk contains the sample data:
  big         36          4 bytes     'data'        // 0x64617461
  little      40          4 bytes     <length of the data block>
  little      44          *           <sample data>
@endcode
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/** @addtogroup STM32100B_EVAL_Demo
  * @{
  */

/** @addtogroup WAVEPLAYER
  * @{
  */

/** @defgroup WAVEPLAYER_Exported_Types
  * @{
  */
typedef struct
{
  uint32_t  RIFFchunksize;
  uint16_t  FormatTag;
  uint16_t  NumChannels;
  uint32_t  SampleRate;
  uint32_t  ByteRate;
  uint16_t  BlockAlign;
  uint16_t  BitsPerSample;
  uint32_t  DataSize;
}
WAVE_FormatTypeDef;
typedef enum
{
  Valid_WAVE_File = 0,
  Unvalid_RIFF_ID,
  Unvalid_WAVE_Format,
  Unvalid_FormatChunk_ID,
  Unsupporetd_FormatTag,
  Unsupporetd_Number_Of_Channel,
  Unsupporetd_Sample_Rate,
  Unsupporetd_Bits_Per_Sample,
  Unvalid_DataChunk_ID,
  Unsupporetd_ExtraFormatBytes,
  Unvalid_FactChunk_ID
} ErrorCode;

/**
  * @}
  */



/** @defgroup WAVEPLAYER_Exported_Constants
  * @{
  */
#define SpeechReadAddr         0x0  /* Speech wave start read address */


/* Audio Play STATUS */
#define AudioPlayStatus_STOPPED       0
#define AudioPlayStatus_PLAYING       1
#define AudioPlayStatus_PAUSED        2  

#define MAX_WAVE_FILES       25

/**
  * @}
  */


/** @defgroup WAVEPLAYER_Exported_Macros
  * @{
  */
/**
  * @}
  */



/** @defgroup WAVEPLAYER_Exported_functions
  * @{
  */
void WavePlayer_Init(void);
uint8_t WavePlayer_Start(void);
void WavePlayer_Stop(void);
void WavePlayer_RePlay(void);
void WavePlayer_Pause(void);
void WavePointerUpdate(uint32_t value);
uint32_t Decrement_WaveDataLength(void);
void Set_WaveDataLength(uint32_t value);
ErrorCode Get_WaveFileStatus(void);
uint8_t WavePlayerMenu_Start(uint8_t *DirName, uint8_t *FileName);
uint8_t LCD_Update(void);

#ifdef __cplusplus
}
#endif

#endif /*__WAVEPLAYER_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
  
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
