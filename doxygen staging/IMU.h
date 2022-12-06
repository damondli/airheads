/** @file IMU.h
 *  @brief This file contains the headers for interfacing with 
 *         the LISM3MDL magnetometer chip and the LSM6DSOX accelerometer 
 *         and gyroscope chip. 
 * 
 * @author Daniel Xu and the Aiheads Team
 * @date 2022-Nov-28 Original file
 */
#include <Arduino.h>
#include <Wire.h>
#include "PrintStream.h"
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <time.h>

#ifndef _IMU_H_
#define _IMU_H_

/// @brief Class to interface witch the magnetometer
class LIS3MDL
{
protected:
    uint8_t _LIS3MDLAddress = 0x1E;

    // register addresses
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

    void writeRegister(byte Register, byte RegData); ///< header function to write to registers
    uint8_t readRegister(byte Register); ///< header function to read from registers

    enum OM{LPM = 0b00 ,MPM = 0b01 ,HPM =0b10,UHPM = 0b11}; ///< enum to select operating mode
    enum FS{GAUSS_4 = 0b00, GAUSS_8 = 0b01, GAUSS_12 = 0b10, GAUSS_16 = 0b11}; ///< enum to select full selection scale
    enum MD{CONTINUOUS_CONVERSION = 0b00, SINGLE_CONVERSION = 0b01, POWER_DOWN = 0b11}; ///< enum for mode selection
    enum BLE{LSB = 0b1, MSB = 0b1}; ///< enum for BLE selection

public:
    /// @brief Header for LIS3MDL object
    LIS3MDL(TwoWire& i2c, uint8_t address = 0x1E);
    
    /// @brief header to configure register 1
    void config_reg1(bool temp_en = 0, OM OMXY = LPM, uint8_t DOR = 5, bool FAST_ODR = 0, bool ST = 0);
    /// @brief header to configure register 2
    void config_reg2(FS FULL_SCALE = GAUSS_4, bool REBOOT = 0, bool SOFT_RST = 0);
    /// @brief header to configure register 3
    void config_reg3(bool LP = 0, bool SIM = 0, MD SYS_OP_MODE = CONTINUOUS_CONVERSION);
    /// @brief header to configure register 4
    void config_reg4(OM OMZ = LPM, BLE Endian_Data_Selec = LSB);
    /// @brief header to configure register 5
    void config_reg5(bool FAST_READ = 0, bool BDU = 1);
    /// @brief header to read all magnetometer data at once
    void read_xyz_mag(int16_t& MAG_X, int16_t& MAG_Y,int16_t& MAG_Z);    

};
/// @brief Class to interface and get pitch, yaw, roll data from the accelerometer and gyroscope
class LSM6DSOX
{
private:
    Adafruit_LSM6DSOX imu; ///< Create object to use Adafruit libraries
    Adafruit_LIS3MDL Magno; ///< create object to use Adafruit libraries
    float GyroX, GyroY, GyroZ, AccelX, AccelY, AccelZ; ///< Initializing variables to get gyro and accel data
    float pitch = 0; ///< initial value for pitch
    float yaw = 0; ///< initial value for yaw
    float roll = 0; ///< initial value for roll
    float last_time = 0;
    int16_t MAGX, MAGY, MAGZ; ///< initialize for raw MAG X, Y, and Z data
    int16_t nMAGX, nMAGY, nMAGZ;///< initialize for tilt compensated MAG X, Y, and Z data

    float yaw_offset = 0; ///< initial value for yaw offset
    float roll_offset = 0; ///< initial value for roll offset
    float pitch_offset= 0; ///< initial value for pitch offset
    


public:
    /// @brief header function for LSM6DSOX to initialize object  
    LSM6DSOX(void);

    /// @brief header function to read gyro and accelerometer data
    void read_data(float& GYRO_X, float& GYRO_Y,float& GYRO_Z,float& ACCEL_X, 
                    float& ACCEL_Y,float& ACCEL_Z);

    /// @brief header function to get pitch, yaw, and roll data
    void get_angle(float time, float& pitch, float& yaw, float& roll);

    /// @brief header function to zero yaw 
    void zero(void);

};

#endif //_IMU_H_
