#ifndef MPU9250_H__
#define MPU9250_H__

#include <stdint.h>

uint8_t MPU9250_readWhoAmI(void);
bool MPU9250_InitProcedure(void);
bool MPU9250_CheckNewSample(void);
void MPU9250_Get9AxisData(float accel[3], float gyro[3], float mag[3]);
float MPU9250_GetTemperature(void);
void MPU9250_CalcOrientation(float *yaw, float *pitch, float *roll);


#endif