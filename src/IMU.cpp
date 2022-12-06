/** @file IMU.cpp
 *  @brief This file contains the implementation code for interfacing with 
 *         the LISM3MDL magnetometer chip and the LSM6DSOX accelerometer 
 *         and gyroscope chip. 
 * 
 * @author Daniel Xu and the Airheads Team
 * @date 2022-Nov-28 Original file
 */
#include <Arduino.h>
#include "IMU.h"
#include "PrintStream.h"

/// @brief Constructor for LIS3MDL object, which operates with the magnetometer
/// @param i2c Default two wire method, default set to Wire
/// @param address Address for i2c communication, default set to 0x1E
LIS3MDL::LIS3MDL(TwoWire& i2c, uint8_t address)
{
    // Checks to see if address is right, if not switch to alternative address
    byte error;
    p_i2c = &i2c;
    p_i2c-> beginTransmission((byte)address);
    error = p_i2c->endTransmission();

    switch(error){
        case 0:
            _LIS3MDLAddress = address;
            break;
        default:
            _LIS3MDLAddress = 0x1C;
    }
    Serial.println(_LIS3MDLAddress,HEX);

    config_reg1();
    config_reg2();
    config_reg3();
    config_reg4();
    config_reg5();
}


/// @brief Function to configure Register 1
/// @param temp_en Bool to enable temp sensor or not
/// @param OMXY Set operating mode of XY sensors
/// @param DOR Set data output rate
/// @param FAST_ODR Bool to enable fast output data rate
/// @param ST Bool to enable, disable ST
void LIS3MDL::config_reg1(bool temp_en, OM OMXY, uint8_t DOR, bool FAST_ODR, bool ST)
{
    byte _settings;
    _settings |= temp_en << 7;

    _settings |= (OMXY << 5);

    switch(DOR){
        case 1: // 0.625 Hz
            _settings |= (0b000 << 2);
            break;
        case 2: // 1.25 Hz
            _settings |= (0b001 << 2);
            break;
        case 3: // 2.5 Hz
            _settings |= (0b010 << 2);
            break;
        case 4: // 5 Hz
            _settings |= (0b011 << 2);
            break;
        case 5: // 10 Hz
            _settings |= (0b100 << 2);
            break;
        case 6: // 20 Hz
            _settings |= (0b101 << 2);
            break;
        case 7: // 40 Hz
            _settings |= (0b110 << 2);
            break;
        case 8: // 80 Hz
            _settings |= (0b111 << 2);
            break;
    };

    _settings |= FAST_ODR << 1;

    _settings |= ST ;

    writeRegister(_CTRL_REG1,_settings); 
}


/// @brief Function to configure Register 2
/// @param FULL_SCALE Sets the full scale value
/// @param REBOOT Bool to reboot memory content
/// @param SOFT_RST Bool to configure registers and user register reset function
void LIS3MDL::config_reg2(FS FULL_SCALE, bool REBOOT, bool SOFT_RST)
{
    byte _settings;

    // Full Scale Selection (FS)
    _settings |= FULL_SCALE<<5;

    // Reboot Memory Content
    _settings |= REBOOT <<3;

    //Configuration Registers and User Register Reset Function
    _settings |= SOFT_RST<<2;

    writeRegister(_CTRL_REG2,_settings);
}


/// @brief Function to configure Register 3
/// @param LP Set Low-power mode configuration
/// @param SIM Set SPI serial interface mode configuration
/// @param SYS_OP_MODE Set system operating mode selection
void LIS3MDL::config_reg3(bool LP, bool SIM, MD SYS_OP_MODE)
{
    byte _settings;

    // Low-power mode configuration
    _settings |= LP << 5;

    //SPI Serial Interface Mode Selection
    _settings |= SIM << 2;

    //Operationg Mode Selection
    _settings |= SYS_OP_MODE;

    writeRegister(_CTRL_REG3, _settings);
}


/// @brief Function to configure Register 4
/// @param OMZ Z-axis operative mode selection
/// @param Endian_Data_Selec Big/little endian data selection
void LIS3MDL::config_reg4(OM OMZ, BLE Endian_Data_Selec)
{
    byte _settings;

    //Z-axis operative mode selection
    _settings |= OMZ << 2;

    // Big/Little Endian data Selection
    _settings |= Endian_Data_Selec<<1;

    writeRegister(_CTRL_REG4,_settings);
}


/// @brief Function to configure Register 5
/// @param FAST_READ Enable or disable fast read
/// @param BDU Sets block data update for magnetic data
void LIS3MDL::config_reg5(bool FAST_READ, bool BDU)
{
    byte _settings;

    // Set Fast Read
    _settings |= FAST_READ << 7;

    // Set Block Data Update for Magnetic Data
    _settings |= BDU << 6;

    writeRegister(_CTRL_REG5,_settings);
}


/// @brief Reads the data for X, Y, and Z magnetometer data
/// @param MAG_X Reference parameter for X-reading for magnetometer.
/// @param MAG_Y Reference parameter for Y-reading for magnetometer.
/// @param MAG_Z Reference parameter for Z-reading for magnetometer.
void LIS3MDL::read_xyz_mag(int16_t &MAG_X,int16_t &MAG_Y,int16_t &MAG_Z)
{
    p_i2c->beginTransmission(_LIS3MDLAddress);
    p_i2c->write(_OUT_X_L);
    p_i2c->endTransmission();
    uint8_t xyz_reading[6]; 
    p_i2c->requestFrom (_LIS3MDLAddress, (uint8_t)6);
    if (p_i2c->available () >= 6)
    {
        for(uint8_t idx = 0; idx < 6; idx++)
        {
            xyz_reading[idx] = Wire.read();
        }

    MAG_X = xyz_reading[1] << 8| xyz_reading[0];
    MAG_Y = xyz_reading[3] << 8| xyz_reading[2];
    MAG_Z = xyz_reading[5] << 8| xyz_reading[4];
    }
}


/// @brief Writes to register
/// @param Register Register address to write to
/// @param RegData Data to write to address
void LIS3MDL::writeRegister(byte Register, byte RegData)
{
    p_i2c->beginTransmission(_LIS3MDLAddress);
    p_i2c->write(Register);
    p_i2c->write(RegData);
    p_i2c->endTransmission();
}


/// @brief Reads from register
/// @param Register Register to read from
/// @returns Reading from register
uint8_t LIS3MDL::readRegister(byte Register)
{
    p_i2c->beginTransmission(_LIS3MDLAddress);
    p_i2c->write(Register);
    p_i2c->endTransmission();
    
    uint8_t _reading = 0xFF;
    p_i2c->requestFrom (_LIS3MDLAddress, (uint8_t)1);
    if (p_i2c->available () <= 1)
    {
        _reading = p_i2c->read ();
    }

    return _reading;
}


/// @brief Constructor for LSM6DSOX object, which handles the accelerometer and gyroscope sensors
LSM6DSOX::LSM6DSOX(void)
{
    // For initial setup for i2C communication, set up i2c using the Adafruit libraray method
    if (!imu.begin_I2C()) {

        while (1) {
        delay(10);
        }
    }

    // Set mode to start with continuous mode, which continuously collects data
    Magno.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    // Collect data as fast as possible
    Magno.setDataRate(LIS3MDL_DATARATE_1000_HZ);

    Serial.println("LSM6DSOX Initialized");
}


/// @brief Reads the data for gyroscope and accelerometer
/// @param GYRO_X Reference parameter for Gyro X reading in rad/s
/// @param GYRO_Y Reference parameter for Gyro Y reading in rad/s
/// @param GYRO_Z Reference parameter for Gyro Z reading in rad/s
/// @param ACCEL_X Reference parameter for Accelerometer X reading in m/s^2
/// @param ACCEL_Y Reference parameter for Accelerometer Y reading in m/s^2
/// @param ACCEL_Z Reference parameter for Accelerometer Z reading in m/s^2
void LSM6DSOX::read_data(float& GYRO_X, float& GYRO_Y,float& GYRO_Z,float& ACCEL_X, float& ACCEL_Y,float& ACCEL_Z)
{
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;

    imu.getEvent(&accel, &gyro, &temp);

    // units: m/s^2
    ACCEL_X = accel.acceleration.x;
    ACCEL_Y = accel.acceleration.y;
    ACCEL_Z = accel.acceleration.z;

    // units: rad/s
    GYRO_X = gyro.gyro.x;
    GYRO_Y = gyro.gyro.y;
    GYRO_Z = gyro.gyro.z;

}


/// @brief Calculates the pitch, yaw, and roll from sensor data
/// @param new_time Time at which the function is called using time.h
/// @param pitch_in Reference parameter to pitch
/// @param yaw_in Reference parameter to yaw
/// @param roll_in Reference parameter for roll_in
void LSM6DSOX::get_angle(float new_time, float& pitch_in, float& yaw_in, float& roll_in)
{
    // Read magnetometer data
    sensors_event_t event; 
    Magno.getEvent(&event);
    MAGX = event.magnetic.x;
    MAGY = event.magnetic.y;
    MAGZ = event.magnetic.z;


    // https://www.analog.com/en/app-notes/an-1057.html
    float theta, psi, phi;
    //
    if(last_time == 0)
    {
        last_time = new_time;
    }
    else
    {
        // used to find change in time to use in integrating gyroscope
        float dt = difftime(new_time, last_time);
        
        // read data values for gyro and accelerometer
        read_data(GyroX, GyroY, GyroZ, AccelX, AccelY, AccelZ);

        // calculate phi and psi for pitch and roll
        // phi is used to determine pitch
        // psi is used to determine roll
        phi = atan2(AccelX,(sqrt(AccelY*AccelY + AccelZ*AccelZ)));
        psi = atan2(AccelY,(sqrt(AccelX*AccelX + AccelZ*AccelZ)));
        

        // Calculate pitch and roll
        // Equations for both integration of gyroscope with accelerometer
        // and equations that do not use gyroscope

        // equations with gyro integration
        // pitch_in = 0.98*(pitch + GyroX*dt) + 0.02*phi - pitch_offset; 
        // roll_in = 0.98*(roll + GyroY*dt) + 0.02*psi - roll_offset;   

        // equations without gyro integration   
        pitch_in = round((0.00*(pitch + GyroX*dt) + 1.00*phi - pitch_offset) * 180/M_PI) * M_PI/180;
        roll_in = round((0.00*(roll + GyroY*dt) + 1.00*psi - roll_offset) * 180/M_PI) * M_PI/180;

        // Calculates new Magnetometer angles with tilt compensations
        nMAGX = MAGX*cos(pitch) + MAGZ*sin(pitch);
        nMAGY = MAGX*sin(roll) * sin(pitch) + MAGZ*sin(roll)*cos(pitch);
        
        // calculates yaw using new magnetometer readings
        yaw_in = atan2(-nMAGY,nMAGX) - yaw_offset;

        // sets pitch, roll, and yaw to be stored for future use
        pitch = pitch_in;
        roll = roll_in;
        yaw = yaw_in;
    }
    last_time = new_time;
}


/// @brief Sets current yaw angle to be the offset
void LSM6DSOX::zero(void)
{
    yaw_offset = yaw; 
}