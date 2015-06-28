#include "tmp102.h"
#include "i2c.h"

#define TMP102_I2C_ADDRESS 72 /* This is the I2C address for our chip.
This value is correct if you tie the ADD0 pin to ground. See the datasheet for some other values. */

float TMP102_GetTemp()
{
    uint8_t firstbyte, secondbyte; //these are the bytes we read from the TMP102 temperature registers
    int val; /* an int is capable of storing two bytes, this is where we "chuck" the two bytes together. */
    float convertedtemp; /* We then need to multiply our two bytes by a scaling factor, mentioned in the datasheet. */
    float correctedtemp;
    // The sensor overreads (?)


    /* Reset the register pointer (by default it is ready to read temperatures)
    You can alter it to a writeable register and alter some of the configuration -
    the sensor is capable of alerting you if the temperature is above or below a specified threshold. */

    uint8_t data[2] = {0x00};
    I2C_Lib_MasterRW(I2C_OP_Write, TMP102_I2C_ADDRESS, 1, data);
    I2C_Lib_WaitForRWDone();
    I2C_Lib_MasterRW(I2C_OP_Read, TMP102_I2C_ADDRESS, 2, data);
    I2C_Lib_WaitForRWDone();


    firstbyte      = (data[0]);
    /*read the TMP102 datasheet - here we read one byte from
     each of the temperature registers on the TMP102*/
    secondbyte     = (data[1]);
    /*The first byte contains the most significant bits, and
     the second the less significant */
    val = ((firstbyte) << 4);
    /* MSB */
    val |= (secondbyte >> 4);
    /* LSB is ORed into the second 4 bits of our byte.
    Bitwise maths is a bit funky, but there's a good tutorial on the playground*/
    convertedtemp = val * 0.0625;
    correctedtemp = convertedtemp - 5;
    /* See the above note on overreading */


    // Serial.print("firstbyte is ");
    // Serial.print("\t");
    // Serial.println(firstbyte, BIN);
    // Serial.print("secondbyte is ");
    // Serial.print("\t");
    // Serial.println(secondbyte, BIN);
    // Serial.print("Concatenated byte is ");
    // Serial.print("\t");
    // Serial.println(val, BIN);
    // Serial.print("Converted temp is ");
    // Serial.print("\t");
    // Serial.println(val * 0.0625);
    // Serial.print("Corrected temp is ");
    // Serial.print("\t");
    // Serial.println(correctedtemp);
    // Serial.println();
    return correctedtemp;
}
