/** potentiometer.h
 * 
 * 
 * 
 * 
 * 
 */

// Define this header file only once
#ifndef _POT_
#define _POT_

// Include appropriate modules
#include <Arduino.h>

class Potentiometer
{
protected:
    // Pin to read voltage from
    uint8_t ADC_PIN;
    // Range of ADC values determined by resolution (12 bit default)
    const uint16_t ADC_RANGE = 4096;
    // Voltage source to scale the ADC reading
    const float VOLTAGE_SOURCE = 3.3;

    // Offset to zero the potentiometer
    float voltage_offset;

    // Voltage to angle conversion
    float VOLTAGE_TO_DEGREES = 60;

public:
    // Setup object
    Potentiometer(uint8_t pin, float offset);

    // Value from ADC reading
    uint16_t adc_value;
    
    // Voltage from ADC reading
    float voltage;

    // Get the position of the potentiometer
    float get_voltage(void);

    // Get the position of the potentiometer
    float get_angle(void);

    // Zero the potentiometer
    void zero(void);
};

#endif // _POT_