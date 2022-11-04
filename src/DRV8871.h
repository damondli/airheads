/** DRV8871.h
 * 
 * 
 * 
 * 
 * 
 */

#ifndef _DRV8871_H_
#define _DRV8871_H_

#include <Arduino.h>

class DRV8871
{
protected:
    // Variables for declaring each motor
    uint8_t PIN_A;
    uint8_t PIN_B;
    uint8_t CHANNEL_A;
    uint8_t CHANNEL_B;

    // PWM properties
    const uint16_t frequency = 5000; //Hz
    const uint8_t resolution = 8;
    const uint8_t max_duty = uint8_t (pow(2, resolution) - 1);

    // Rated voltage cap
    const uint8_t voltage_source = 9;
    const uint8_t rated_voltage = 5;




public:
    DRV8871(uint8_t pin_A, uint8_t pin_B, uint8_t channel_A, uint8_t channel_B);

    int16_t duty;

    void enable (void);
    void disable (void);
    void set_duty (int16_t);
};

#endif // _DRV8871_H_