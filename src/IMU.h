#include <Arduino.h>
#include <Wire.h>
#include "PrintStream.h"
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <time.h>

#ifndef _IMU_H_
#define _IMU_H_

/** @brief   
 * 
 */
class LIS3MDL
{
protected:
    uint8_t _LIS3MDLAddress = 0x1E;

    const byte _CTRL_REG1 = 0x20;
    const byte _CTRL_REG2 = 0x21;
    const byte _CTRL_REG3 = 0x22;
    const byte _CTRL_REG4 = 0x23;
    const byte _CTRL_REG5 = 0x24;
    const byte _STATUS_REG = 0x27;
    const byte _OUT_X_L = 0x28;
    const byte _OUT_X_H = 0x29;
    const byte _OUT_Y_L = 0x2A;
    const byte _OUT_Y_H = 0x2B;
    const byte _OUT_Z_L = 0x2C;
    const byte _OUT_Z_H = 0x2D;
    const byte _INT_CFG = 0x30;
    const byte _INT_THS_L = 0x32;
    const byte _INT_THS_H = 0x33;

    TwoWire* p_i2c;

    void writeRegister(byte Register, byte RegData);
    uint8_t readRegister(byte Register);

    enum OM{LPM = 0b00 ,MPM = 0b01 ,HPM =0b10,UHPM = 0b11};
    enum FS{GAUSS_4 = 0b00, GAUSS_8 = 0b01, GAUSS_12 = 0b10, GAUSS_16 = 0b11};
    enum MD{CONTINUOUS_CONVERSION = 0b00, SINGLE_CONVERSION = 0b01, POWER_DOWN = 0b11};
    enum BLE{LSB = 0b1, MSB = 0b1};

public:
    LIS3MDL(TwoWire& i2c, uint8_t address = 0x1E);
    
    void config_reg1(bool temp_en = 0, OM OMXY = LPM, uint8_t DOR = 5, bool FAST_ODR = 0, bool ST = 0);
    void config_reg2(FS FULL_SCALE = GAUSS_4, bool REBOOT = 0, bool SOFT_RST = 0);
    void config_reg3(bool LP = 0, bool SIM = 0, MD SYS_OP_MODE = CONTINUOUS_CONVERSION);
    void config_reg4(OM OMZ = LPM, BLE Endian_Data_Selec = LSB);
    void config_reg5(bool FAST_READ = 0, bool BDU = 1);
    void read_xyz_mag(int16_t& MAG_X, int16_t& MAG_Y,int16_t& MAG_Z);    
    /*
    float magnetic_heading(void);
    */
};

class LSM6DSOX
{
private:
    Adafruit_LSM6DSOX imu;
    Adafruit_LIS3MDL Magno;
    float GyroX, GyroY, GyroZ, AccelX, AccelY, AccelZ;
    float pitch = 0;
    float yaw = 0;
    float roll = 0;
    float last_time = 0;
    float grav = 9.81;
    int16_t MAGX, MAGY, MAGZ;
    int16_t nMAGX, nMAGY, nMAGZ;
    float _init_Angle = 0;
    


public:
    LSM6DSOX(void);

    void read_data(float& GYRO_X, float& GYRO_Y,float& GYRO_Z,float& ACCEL_X, 
                    float& ACCEL_Y,float& ACCEL_Z);

    void get_angle(float time, float& pitch, float& yaw, float& roll);

};

#endif //_IMU_H_
