#include <Arduino.h>
#include "IMU.h"
#include "PrintStream.h"

LIS3MDL::LIS3MDL(TwoWire& i2c, uint8_t address)
{
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

void LIS3MDL::config_reg4(OM OMZ, BLE Endian_Data_Selec)
{
    byte _settings;

    //Z-axis operative mode selection
    _settings |= OMZ << 2;

    // Big/Little Endian data Selection
    _settings |= Endian_Data_Selec<<1;

    writeRegister(_CTRL_REG4,_settings);
}

void LIS3MDL::config_reg5(bool FAST_READ, bool BDU)
{
    byte _settings;

    // Set Fast Read
    _settings |= FAST_READ << 7;

    // Set Block Data Update for Magnetic Data
    _settings |= BDU << 6;

    writeRegister(_CTRL_REG5,_settings);


}

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

/*
float LIS3MDL::magnetic_heading(void)
{

}
*/

void LIS3MDL::writeRegister(byte Register, byte RegData)
{
    p_i2c->beginTransmission(_LIS3MDLAddress);
    p_i2c->write(Register);
    p_i2c->write(RegData);
    p_i2c->endTransmission();
}

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

LSM6DSOX::LSM6DSOX(void)
{
    if (!imu.begin_I2C()) {

        while (1) {
        delay(10);
        }
    }
    Magno.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    Magno.setDataRate(LIS3MDL_DATARATE_1000_HZ);

    Serial.println("LSM6DSOX Initialized");


}


void LSM6DSOX::read_data(float& GYRO_X, float& GYRO_Y,float& GYRO_Z,float& ACCEL_X, 
    float& ACCEL_Y,float& ACCEL_Z)
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

void LSM6DSOX::get_angle(float new_time, float& pitch_in, float& yaw_in, float& roll_in)
{
    sensors_event_t event; 
    Magno.getEvent(&event);
    MAGX = event.magnetic.x;
    MAGY = event.magnetic.y;
    MAGZ = event.magnetic.z;


    // https://www.analog.com/en/app-notes/an-1057.html

    float theta, psi, phi;
    if(last_time == 0)
    {
        last_time = new_time;
    }
    else
    {
        float dt = difftime(new_time, last_time);
        
        read_data(GyroX, GyroY, GyroZ, AccelX, AccelY, AccelZ);

        
        phi = atan2(AccelX,(sqrt(AccelY*AccelY + AccelZ*AccelZ)));
        psi = atan2(AccelY,(sqrt(AccelX*AccelX + AccelZ*AccelZ)));
        

        // pitch_in = 0.98*(pitch + GyroX*dt) + 0.02*phi - pitch_offset;
        // roll_in = 0.98*(roll + GyroY*dt) + 0.02*psi - roll_offset;
        pitch_in = round((0.00*(pitch + GyroX*dt) + 1.00*phi - pitch_offset) * 180/M_PI) * M_PI/180;
        roll_in = round((0.00*(roll + GyroY*dt) + 1.00*psi - roll_offset) * 180/M_PI) * M_PI/180;


        nMAGX = MAGX*cos(pitch) + MAGZ*sin(pitch);
        nMAGY = MAGX*sin(roll) * sin(pitch) + MAGZ*sin(roll)*cos(pitch);
        
        yaw_in = atan2(-nMAGY,nMAGX) - yaw_offset;

        pitch = pitch_in;
        roll = roll_in;
        yaw = yaw_in;
    }
    last_time = new_time;
}

void LSM6DSOX::zero(void)
{
    yaw_offset = yaw; 
}



