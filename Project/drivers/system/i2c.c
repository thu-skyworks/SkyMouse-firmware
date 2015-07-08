#include "i2c.h"
#include "common.h"
#include "cpal_i2c.h"


void I2C_Lib_Init()
{
    static bool initialized = false;
    if(initialized)
        return;

    CPAL_I2C_StructInit(&I2C_HOST_DEV);

    // I2C_HOST_DEV.CPAL_Dev is already initialized in definition
    I2C_HOST_DEV.wCPAL_Options = CPAL_OPT_NO_MEM_ADDR;
    I2C_HOST_DEV.CPAL_ProgModel = CPAL_PROGMODEL_INTERRUPT;
    // I2C_HOST_DEV.pCPAL_I2C_Struct->I2C_ClockSpeed = 350000;
    // I2C_HOST_DEV.pCPAL_I2C_Struct->I2C_OwnAddress1 = 0xAA;

    uint32_t ret = CPAL_I2C_Init(&I2C_HOST_DEV);
    if(ret != CPAL_PASS){
        ERR_MSG("CPAL_I2C_Init Failed");
        return;
    }
    initialized = true;
}

void CPAL_I2C_ERR_UserCallback(CPAL_DevTypeDef pDevInstance, uint32_t Device_Error) 
{
    DBG_MSG("Error: %d", Device_Error);
}


