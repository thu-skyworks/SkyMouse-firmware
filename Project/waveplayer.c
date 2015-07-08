/**
  ******************************************************************************
  * @file    waveplayer.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    08-April-2011
  * @brief   This file includes the wave player driver for the STM32100B-EVAL demo.
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

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "waveplayer.h"
#include "ff.h"

#define SECTOR_SIZE 512

#define __REV(num)  (((num>>24)&0xff) | \
                    ((num<<8)&0xff0000) | \
                    ((num>>8)&0xff00) | \
                    ((num<<24)&0xff000000))
/** @addtogroup STM32100B_EVAL_Demo
  * @{
  */

/** @defgroup WAVEPLAYER
  * @brief    This file includes the wave player driver for the STM32100B-EVAL
  *           demo.
  * @{
  */

/** @defgroup WAVEPLAYER_Private_Types
  * @{
  */
typedef enum {
    LittleEndian,
    BigEndian
} Endianness;
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Defines
  * @{
  */
#define  CHUNK_ID                            0x52494646  /* correspond to the letters 'RIFF' */
#define  FILE_FORMAT                         0x57415645  /* correspond to the letters 'WAVE' */
#define  FORMAT_ID                           0x666D7420  /* correspond to the letters 'fmt ' */
#define  DATA_ID                             0x64617461  /* correspond to the letters 'data' */
#define  FACT_ID                             0x66616374  /* correspond to the letters 'fact' */
#define  WAVE_FORMAT_PCM                     0x01
#define  FORMAT_CHNUK_SIZE                   0x10
#define  CHANNEL_MONO                        0x01
#define  SAMPLE_RATE_8000                    8000
#define  SAMPLE_RATE_11025                   11025
#define  SAMPLE_RATE_22050                   22050
#define  SAMPLE_RATE_44100                   44100
#define  BITS_PER_SAMPLE_8                   8
#define  WAVE_DUMMY_BYTE                     0xA5
#define  DAC_DHLCD_REG_8LCD_REG_1_ADDRESS    0x40007410

/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Variables
  * @{
  */
static WAVE_FormatTypeDef WAVE_Format;
static __IO ErrorCode WaveFileStatus = Unvalid_RIFF_ID;
static __IO uint16_t TIM6ARRValue = 1088;
__IO uint32_t WaveDataLength = 0;
static __IO uint32_t SpeechDataOffset = 0x00;
static uint32_t wavelen = 0;
__attribute__((__aligned__(4))) uint8_t Wavebuffer2[512];
FIL fiwave;

__attribute__((__aligned__(4))) uint8_t Wavebuffer[512];
__IO uint32_t wavecounter = 0;
uint32_t var, dmaindex = 0;
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_FunctionPrototypes
  * @{
  */
static uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat);
static ErrorCode WavePlayer_WaveParsing(uint8_t *DirName, uint8_t *FileName, uint32_t *FileLen);
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Functions
  * @{
  */

/**
  * @brief  Wave player Initialization
  * @param  None
  * @retval None
  */
void WavePlayer_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DAC_InitTypeDef            DAC_InitStructure;

    /* Configure the Priority Group to 2 bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the TIM6 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the DMA2_Channel3 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | RCC_APB1Periph_TIM6, ENABLE);

    /* TIM6 Configuration */
    TIM_DeInit(TIM6);

    TIM_SetAutoreload(TIM6, TIM6ARRValue);

    /* TIM6 TRGO selection */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    /* Enable TIM6 update interrupt */
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    /* DAC deinitialize */
    DAC_DeInit();
    DAC_StructInit(&DAC_InitStructure);

    /* Fill DAC InitStructure */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

    /* DAC Channel1: 8bit right---------------------------------------------------*/
    /* DAC Channel1 Init */
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* Enable DAC Channel1 */
    DAC_Cmd(DAC_Channel_1, ENABLE);
}

/**
  * @brief  Returns the Wave file status.
  * @param  None
  * @retval Wave file status.
  */
ErrorCode Get_WaveFileStatus(void)
{
    return (WaveFileStatus);
}

/**
  * @brief  Start wave playing
  * @param  None
  * @retval None
  */
uint8_t WavePlayer_Start(void)
{
    WavePlayer_Init();

    /* Read the Speech wave file status */
    WaveFileStatus = WavePlayer_WaveParsing("/", "test.wav", &wavelen);

    if (WaveFileStatus == Valid_WAVE_File) { /* the .WAV file is valid */
        /* Set WaveDataLenght to the Speech wave length */
        WaveDataLength = WAVE_Format.DataSize;

        TIM_SetAutoreload(TIM6, TIM6ARRValue);
        DBG_MSG("TIM6ARRValue=%d", TIM6ARRValue);

        /* Start TIM6 */
        TIM_Cmd(TIM6, ENABLE);
    } else {
        ERR_MSG("Invalid wav file");
        return 1;
    }
    return 0;
}

/**
  * @brief  Stop wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_Stop(void)
{
    DBG_MSG("stopped");
    /* Disable TIM6 update interrupt */
    TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
    /* Disable TIM6 */
    TIM_Cmd(TIM6, DISABLE);
}

/**
  * @brief  Pause wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_RePlay(void)
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    /* Enable TIM6 update interrupt */
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
}

/**
  * @brief  Pause wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_Pause(void)
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    /* Disable TIM6 update interrupt */
    TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
}

#if 0
/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
void WavePointerUpdate(uint32_t value)
{
    DFS_Seek(&fiwave, value, Wavebuffer2);
}
#endif

/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
uint32_t Decrement_WaveDataLength(void)
{
    if (WaveDataLength != 0x00) {
        WaveDataLength--;
    }
    return (WaveDataLength);
}


/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
void Set_WaveDataLength(uint32_t value)
{
    WaveDataLength = value;
}

/**
  * @brief  Checks the format of the .WAV file and gets information about
  *   the audio format. This is done by reading the value of a
  *   number of parameters stored in the file header and comparing
  *   these to the values expected authenticates the format of a
  *   standard .WAV  file (44 bytes will be read). If  it is a valid
  *   .WAV file format, it continues reading the header to determine
  *   the  audio format such as the sample rate and the sampled data
  *   size. If the audio format is supported by this application,
  *   it retrieves the audio format in WAVE_Format structure and
  *   returns a zero value. Otherwise the function fails and the
  *   return value is nonzero.In this case, the return value specifies
  *   the cause of  the function fails. The error codes that can be
  *   returned by this function are declared in the header file.
  * @param  None
  * @retval Zero value if the function succeed, otherwise it return
  *         a nonzero value which specifies the error code.
  */
static ErrorCode WavePlayer_WaveParsing(uint8_t *DirName, uint8_t *FileName, uint32_t *FileLen)
{
    uint32_t Temp = 0x00;
    uint32_t ExtraFormatBytes = 0;
    __IO uint32_t err = 0;
    uint32_t number_of_clusters;
    uint32_t i;

    // if (DFS_OpenFile(&vi, FileName, DFS_READ, sector, &fiwave)) {
    if (f_open(&fiwave, FileName, FA_OPEN_EXISTING | FA_READ) != FR_OK) {
        ERR_MSG("Failed to open %s", FileName);
        err = 1;
    } else {
        *FileLen = fiwave.fsize;
        number_of_clusters = fiwave.fsize / 512;
        if ((fiwave.fsize % SECTOR_SIZE) > 0) {
            number_of_clusters ++;
        }
    }
    // DFS_ReadFile(&fiwave, sector, Wavebuffer, &i, SECTOR_SIZE);
    f_read(&fiwave, Wavebuffer, SECTOR_SIZE, &i);

    /* Read chunkID, must be 'RIFF'  ----------------------------------------------*/
    Temp = ReadUnit(Wavebuffer, 0, 4, BigEndian);
    if (Temp != CHUNK_ID) {
        return (Unvalid_RIFF_ID);
    }

    /* Read the file length ----------------------------------------------------*/
    WAVE_Format.RIFFchunksize = ReadUnit(Wavebuffer, 4, 4, LittleEndian);

    /* Read the file format, must be 'WAVE' ------------------------------------*/
    Temp = ReadUnit(Wavebuffer, 8, 4, BigEndian);
    if (Temp != FILE_FORMAT) {
        return (Unvalid_WAVE_Format);
    }

    /* Read the format chunk, must be'fmt ' --------------------------------------*/
    Temp = ReadUnit(Wavebuffer, 12, 4, BigEndian);
    if (Temp != FORMAT_ID) {
        return (Unvalid_FormatChunk_ID);
    }
    /* Read the length of the 'fmt' data, must be 0x10 -------------------------*/
    Temp = ReadUnit(Wavebuffer, 16, 4, LittleEndian);
    if (Temp != 0x10) {
        ExtraFormatBytes = 1;
    }
    /* Read the audio format, must be 0x01 (PCM) -------------------------------*/
    WAVE_Format.FormatTag = ReadUnit(Wavebuffer, 20, 2, LittleEndian);
    if (WAVE_Format.FormatTag != WAVE_FORMAT_PCM) {
        return (Unsupporetd_FormatTag);
    }

    /* Read the number of channels, must be 0x01 (Mono) ------------------------*/
    WAVE_Format.NumChannels = ReadUnit(Wavebuffer, 22, 2, LittleEndian);
    if (WAVE_Format.NumChannels != CHANNEL_MONO) {
        return (Unsupporetd_Number_Of_Channel);
    }

    /* Read the Sample Rate ----------------------------------------------------*/
    WAVE_Format.SampleRate = ReadUnit(Wavebuffer, 24, 4, LittleEndian);
    /* Update the OCA value according to the .WAV file Sample Rate */
    switch (WAVE_Format.SampleRate) {
    case SAMPLE_RATE_8000 :
        TIM6ARRValue = 9000;
        break; /* 8KHz = 72MHz / 9000 */
    case SAMPLE_RATE_11025:
        TIM6ARRValue = 6528;
        break; /* 11.025KHz = 72MHz / 6528 */
    case SAMPLE_RATE_22050:
        TIM6ARRValue = 3264;
        break; /* 22.05KHz = 72MHz / 3264 */
    case SAMPLE_RATE_44100:
        TIM6ARRValue = 1632;
        break; /* 44.1KHz = 72MHz / 1632 */
    default:
        return (Unsupporetd_Sample_Rate);
    }

    /* Read the Byte Rate ------------------------------------------------------*/
    WAVE_Format.ByteRate = ReadUnit(Wavebuffer, 28, 4, LittleEndian);

    /* Read the block alignment ------------------------------------------------*/
    WAVE_Format.BlockAlign = ReadUnit(Wavebuffer, 32, 2, LittleEndian);

    /* Read the number of bits per sample --------------------------------------*/
    WAVE_Format.BitsPerSample = ReadUnit(Wavebuffer, 34, 2, LittleEndian);
    if (WAVE_Format.BitsPerSample != BITS_PER_SAMPLE_8) {
        return (Unsupporetd_Bits_Per_Sample);
    }
    SpeechDataOffset = 36;
    /* If there is Extra format bytes, these bytes will be defined in "Fact Chunk" */
    if (ExtraFormatBytes == 1) {
        /* Read th Extra format bytes, must be 0x00 ------------------------------*/
        Temp = ReadUnit(Wavebuffer, 36, 2, LittleEndian);
        if (Temp != 0x00) {
            return (Unsupporetd_ExtraFormatBytes);
        }
        /* Read the Fact chunk, must be 'fact' -----------------------------------*/
        Temp = ReadUnit(Wavebuffer, 38, 4, BigEndian);
        if (Temp != FACT_ID) {
            return (Unvalid_FactChunk_ID);
        }
        /* Read Fact chunk data Size ---------------------------------------------*/
        Temp = ReadUnit(Wavebuffer, 42, 4, LittleEndian);

        SpeechDataOffset += 10 + Temp;
    }
    /* Read the Data chunk, must be 'data' ---------------------------------------*/
    Temp = ReadUnit(Wavebuffer, SpeechDataOffset, 4, BigEndian);
    SpeechDataOffset += 4;
    if (Temp != DATA_ID) {
        return (Unvalid_DataChunk_ID);
    }

    /* Read the number of sample data ------------------------------------------*/
    WAVE_Format.DataSize = ReadUnit(Wavebuffer, SpeechDataOffset, 4, LittleEndian);
    SpeechDataOffset += 4;
    wavecounter =  SpeechDataOffset;
    return (Valid_WAVE_File);
}

/**
  * @brief  Start wave playing
  * @param  None
  * @retval None
  */
uint8_t WavePlayerMenu_Start(uint8_t *DirName, uint8_t *FileName)
{
    DAC_InitTypeDef            DAC_InitStructure;
    DMA_InitTypeDef            DMA_InitStructure;
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef           NVIC_InitStructure;
    uint32_t var;
    uint8_t tmp;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | RCC_APB1Periph_TIM6, ENABLE);

    WaveFileStatus = Unvalid_RIFF_ID;

    /* TIM6 Configuration */
    TIM_DeInit(TIM6);

    /* DMA2 channel3 configuration */
    DMA_DeInit(DMA2_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHLCD_REG_8LCD_REG_1_ADDRESS;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & Wavebuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 512;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);

    /* Enable DMA2 Channel3 */
    DMA_Cmd(DMA2_Channel3, ENABLE);

    /* DAC deinitialize */
    DAC_DeInit();
    DAC_StructInit(&DAC_InitStructure);

    /* Fill DAC InitStructure */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

    /* DAC Channel1: 8bit right---------------------------------------------------*/
    /* DAC Channel1 Init */
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
       automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    /* Enable DMA for DAC Channel1 */
    DAC_DMACmd(DAC_Channel_1, ENABLE);

    /* Read the Speech wave file status */
    WaveFileStatus = WavePlayer_WaveParsing(DirName, FileName, &wavelen);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

    /* TIM6 TRGO selection */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
    TIM_SetAutoreload(TIM6, TIM6ARRValue);

    TIM_DMACmd(TIM6, TIM_DMA_Update, ENABLE);

    if (WaveFileStatus == Valid_WAVE_File) { /* the .WAV file is valid */
        /* Set WaveDataLenght to the Speech wave length */
        WaveDataLength = WAVE_Format.DataSize;
    } else {
        ERR_MSG("No valid WAV files ErrorCode=%d", WaveFileStatus);
        return 1;
    }

    /* Start TIM6 */
    TIM_Cmd(TIM6, ENABLE);

    /* Enable the DMA2_Channel3 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // LCD_SetTextColor(LCD_COLOR_MAGENTA);
    /* Set the Back Color */
    // LCD_SetBackColor(LCD_COLOR_BLUE);
    // LCD_DrawRect(LCD_LINE_7, 310, 16, 300);

    while (WaveDataLength) {
        // DFS_ReadFile(&fiwave, sector, Wavebuffer2, &var, SECTOR_SIZE);
        f_read(&fiwave, Wavebuffer2, SECTOR_SIZE, &var);
        DBG_MSG("read1: %d", var);
        if (WaveDataLength) WaveDataLength -= 512;
        if (WaveDataLength < 512) WaveDataLength = 0;
        while (DMA_GetFlagStatus(DMA2_FLAG_TC3) == RESET) {
            tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
            // LCD_SetTextColor(LCD_COLOR_MAGENTA);
            // LCD_DrawLine(LCD_LINE_7, 310 - ((tmp) * 3), 16, LCD_DIR_VERTICAL);
        }
        DMA2->IFCR = DMA2_FLAG_TC3;
        DMA2_Channel3->CCR = 0x0;

        // KeyState = LCD_Update();
        // if (KeyState == DOWN) {
        //     return DOWN;
        // }
        DMA2_Channel3->CNDTR = 0x200;
        DMA2_Channel3->CPAR = 0x40007410;
        DMA2_Channel3->CMAR = (uint32_t) & Wavebuffer2;
        DMA2_Channel3->CCR = 0x2091;

        // DFS_ReadFile(&fiwave, sector, Wavebuffer, &var, SECTOR_SIZE);
        f_read(&fiwave, Wavebuffer, SECTOR_SIZE, &var);
        DBG_MSG("read2: %d", var);
        if (WaveDataLength) WaveDataLength -= 512;
        if (WaveDataLength < 512) WaveDataLength = 0;

        while (DMA_GetFlagStatus(DMA2_FLAG_TC3) == RESET) {
            tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
            // LCD_SetTextColor(LCD_COLOR_MAGENTA);
            // LCD_DrawLine(LCD_LINE_7, 310 - ((tmp) * 3), 16, LCD_DIR_VERTICAL);
        }
        DMA2->IFCR = DMA2_FLAG_TC3;
        DMA2_Channel3->CCR = 0x0;

        // KeyState = LCD_Update();
        // if (KeyState == DOWN) {
        //     return DOWN;
        // }

        DMA2_Channel3->CNDTR = 0x200;
        DMA2_Channel3->CPAR = 0x40007410;
        DMA2_Channel3->CMAR = (uint32_t) & Wavebuffer;
        DMA2_Channel3->CCR = 0x2091;
    }
    DMA2_Channel3->CCR = 0x0;

    /* Disable TIM6 */
    TIM_Cmd(TIM6, DISABLE);
    WaveDataLength = 0;

    // LCD_SetTextColor(LCD_COLOR_WHITE);
    // LCD_DisplayStringLine(LCD_LINE_3, "SEL  -> Play        ");
    // LCD_DisplayStringLine(LCD_LINE_4, "DOWN -> Exit        ");
    // LCD_DisplayStringLine(LCD_LINE_5, "LEFT -> Next Wave   ");
    // LCD_DisplayStringLine(LCD_LINE_6, "RIGHT->Previous Wave");
    // LCD_SetBackColor(LCD_COLOR_BLUE);
    // LCD_ClearLine(LCD_LINE_7);

    DBG_MSG("Finished!");
    return 0;
}

/**
  * @brief  Reads a number of bytes from the SPI Flash and reorder them in Big
  *         or little endian.
  * @param  NbrOfBytes: number of bytes to read.
  *         This parameter must be a number between 1 and 4.
  * @param  ReadAddr: external memory address to read from.
  * @param  Endians: specifies the bytes endianness.
  *         This parameter can be one of the following values:
  *             - LittleEndian
  *             - BigEndian
  * @retval Bytes read from the SPI Flash.
  */
static uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat)
{
    uint32_t index = 0;
    uint32_t Temp = 0;

    for (index = 0; index < NbrOfBytes; index++) {
        Temp |= buffer[idx + index] << (index * 8);
    }

    if (BytesFormat == BigEndian) {
        Temp = __REV(Temp);
    }
    return Temp;
}

#if 0
void WavePlayer_TIM_IT_Handler(void)
{
    DBG_MSG("called");
    /* Set DAC Channel1 DHR register */
    DAC_SetChannel1Data(DAC_Align_8b_R, Wavebuffer[wavecounter++]);
    if (wavecounter == 511) {
        wavecounter = 0;
        // DFS_ReadFile(&fiwave, sector, Wavebuffer, &var, SECTOR_SIZE);
        f_read(&fiwave, Wavebuffer, SECTOR_SIZE, &var);
        DBG_MSG("read: %d", var);
    }

    /* If we reach the WaveDataLength of the wave to play */
    if (Decrement_WaveDataLength() == 0) {
        /* Stop wave playing */
        WavePlayer_Stop();
    }

}
#endif

void WavePlayer_DMA_IT_Handler(void)
{
    DBG_MSG("called");
    if (dmaindex == 0) {
        DMA2_Channel3->CCR = 0x0;
        DMA2_Channel3->CNDTR = 0x200;
        DMA2_Channel3->CPAR = 0x40007410;
        DMA2_Channel3->CMAR = (uint32_t) & Wavebuffer2;
        DMA2_Channel3->CCR = 0x2093;
        // DFS_ReadFile(&fiwave, sector, Wavebuffer, &var, SECTOR_SIZE);
        f_read(&fiwave, Wavebuffer, SECTOR_SIZE, &var);
        dmaindex++;
    } else {
        DMA2_Channel3->CCR = 0x0;
        DMA2_Channel3->CNDTR = 0x200;
        DMA2_Channel3->CPAR = 0x40007410;
        DMA2_Channel3->CMAR = (uint32_t) & Wavebuffer;
        DMA2_Channel3->CCR = 0x2093;
        // DFS_ReadFile(&fiwave, sector, Wavebuffer2, &var, SECTOR_SIZE);
        f_read(&fiwave, Wavebuffer2, SECTOR_SIZE, &var);
        dmaindex = 0;
    }
    DMA2->IFCR = DMA2_IT_TC3;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
