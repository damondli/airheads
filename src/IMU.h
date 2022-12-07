/** @file IMU.h
 *  @brief This file contains the headers for interfacing with 
 *         the LISM3MDL magnetometer chip and the LSM6DSOX accelerometer 
 *         and gyroscope chip. 
 * 
 * @author Daniel Xu and the Airheads Team
 * @date 2022-Nov-28 Original file
 */

#ifndef _IMU_H_
#define _IMU_H_

#include <Arduino.h>
#include <Wire.h>
#include "PrintStream.h"
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <time.h>

/// @brief Class to interface with the LIS3MDL magnetometer
class LIS3MDL
{
protected:
    uint8_t _LIS3MDLAddress = 0x1E;     ///< Initialize I2C

    // Register addresses
    const byte _CTRL_REG1 = 0x20;       ///< "CTRL_REG1" address
    const byte _CTRL_REG2 = 0x21;       ///< "CTRL_REG2" address
    const byte _CTRL_REG3 = 0x22;       ///< "CTRL_REG3" address
    const byte _CTRL_REG4 = 0x23;       ///< "CTRL_REG4" address
    const byte _CTRL_REG5 = 0x24;       ///< "CTRL_REG5" address
    const byte _STATUS_REG = 0x27;      ///< "STATUS_REG" address
    const byte _OUT_X_L = 0x28;         ///< "OUT_X_L" address for X-axis data output LSB
    const byte _OUT_X_H = 0x29;         ///< "OUT_X_H" address for X-axis data output MSB
    const byte _OUT_Y_L = 0x2A;         ///< "OUT_Y_L" address for Y-axis data output LSB
    const byte _OUT_Y_H = 0x2B;         ///< "OUT_Y_H" address for Y-axis data output MSB
    const byte _OUT_Z_L = 0x2C;         ///< "OUT_Z_L" address for Z-axis data output LSB
    const byte _OUT_Z_H = 0x2D;         ///< "OUT_Z_H" address for Z-axis data output MSB
    const byte _INT_CFG = 0x30;         ///< "INT_CFG" address
    const byte _INT_THS_L = 0x32;       ///< "INT_THS_L" address LSB
    const byte _INT_THS_H = 0x33;       ///< "INT_THS_H" address MSB

    TwoWire* p_i2c;                     ///< Initialize I2C

    void writeRegister(byte Register, byte RegData);                                        ///< Header function to write to registers
    uint8_t readRegister(byte Register);                                                    ///< Header function to read from registers

    enum OM{LPM = 0b00 ,MPM = 0b01 ,HPM =0b10,UHPM = 0b11};                                 ///< Enum to select operating mode
    enum FS{GAUSS_4 = 0b00, GAUSS_8 = 0b01, GAUSS_12 = 0b10, GAUSS_16 = 0b11};              ///< Enum to select full selection scale
    enum MD{CONTINUOUS_CONVERSION = 0b00, SINGLE_CONVERSION = 0b01, POWER_DOWN = 0b11};     ///< Enum for mode selection
    enum BLE{LSB = 0b1, MSB = 0b1};                                                         ///< Enum for BLE selection

public:
    /// @brief Header for LIS3MDL object
    LIS3MDL(TwoWire& i2c, uint8_t address = 0x1E);
    
    /// @brief Header to configure register 1
    void config_reg1(bool temp_en = 0, OM OMXY = LPM, uint8_t DOR = 5, bool FAST_ODR = 0, bool ST = 0);
    /// @brief Header to configure register 2
    void config_reg2(FS FULL_SCALE = GAUSS_4, bool REBOOT = 0, bool SOFT_RST = 0);
    /// @brief Header to configure register 3
    void config_reg3(bool LP = 0, bool SIM = 0, MD SYS_OP_MODE = CONTINUOUS_CONVERSION);
    /// @brief Header to configure register 4
    void config_reg4(OM OMZ = LPM, BLE Endian_Data_Selec = LSB);
    /// @brief Header to configure register 5
    void config_reg5(bool FAST_READ = 0, bool BDU = 1);
    /// @brief Header to read all magnetometer data at once
    void read_xyz_mag(int16_t& MAG_X, int16_t& MAG_Y,int16_t& MAG_Z);    

};
/// @brief Class to interface and get pitch, yaw, roll data from the LSM6DSOX accelerometer and gyroscope
class LSM6DSOX
{
private:
    Adafruit_LSM6DSOX imu;                                  ///< Create object to use Adafruit libraries
    Adafruit_LIS3MDL Magno;                                 ///< Create object to use Adafruit libraries
    float GyroX, GyroY, GyroZ, AccelX, AccelY, AccelZ;      ///< Initializing variables to get gyro and accel data
    float pitch = 0;                                        ///< Initial value for pitch
    float yaw = 0;                                          ///< Initial value for yaw
    float roll = 0;                                         ///< Initial value for roll
    float last_time = 0;
    int16_t MAGX, MAGY, MAGZ;                               ///< Initialize for raw MAG X, Y, and Z data
    int16_t nMAGX, nMAGY, nMAGZ;                            ///< Initialize for tilt compensated MAG X, Y, and Z data

    float yaw_offset = 0;                                   ///< Initial value for yaw offset
    float roll_offset = 0;                                  ///< Initial value for roll offset
    float pitch_offset= 0;                                  ///< Initial value for pitch offset
    


public:
    /// @brief Header function for LSM6DSOX to initialize object  
    LSM6DSOX(void);

    /// @brief Header function to read gyro and accelerometer data
    void read_data(float& GYRO_X, float& GYRO_Y,float& GYRO_Z,float& ACCEL_X, 
                    float& ACCEL_Y,float& ACCEL_Z);

    /// @brief Header function to get pitch, yaw, and roll data
    void get_angle(float time, float& pitch, float& yaw, float& roll);

    /// @brief Header function to zero yaw 
    void zero(void);
};

#endif //_IMU_H_
