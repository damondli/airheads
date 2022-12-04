/** @file potentiometer.h
 * The header file for a potentiometer class which reads the input voltage
 * from a GPIO pin on an ESP32.
 * 
 * @author  Damond Li
 * @date    2022-Nov-03 Original file
 */

// Compile this header file only once
#ifndef _POT_
#define _POT_

// Include appropriate modules
#include <Arduino.h>

/** @brief  Class for a generic potentiometer which is used to determine
 * its position.
 */
class Potentiometer
{
protected:
    // Pin to read voltage from
    uint8_t ADC_PIN;                            ///< Pin to read voltage from (0 - 3.3V)
    // Range of ADC values determined by resolution (12 bit default)
    const uint16_t ADC_RANGE = 4096;            ///< ADC range determined by a default 12 bit resolution
    // Voltage source to scale the ADC reading
    const float VOLTAGE_SOURCE = 3.3;           ///< Voltage source reference to scale ADC reading

    // Offset to zero the potentiometer
    float voltage_offset;                       ///< The offset used to zero the potentiometer

    // Voltage to angle conversion
    float VOLTAGE_TO_DEGREES = 60;              ///< Conversion from voltage to degrees determined experimentally

public:
    // Setup object
    Potentiometer(uint8_t pin, float offset);   ///< Constructor for the potentiometer class

    // Value from ADC reading
    uint16_t adc_value;                         ///< ADC value from the GPIO input pin
    
    // Voltage from ADC reading
    float voltage;                              ///< Input voltage from ADC reading

    // Get the position of the potentiometer
    float get_voltage(void);                    ///< The method to retrieve the voltage at the input pin

    // Get the position of the potentiometer
    float get_angle(void);                      ///< The method to return the position of the potentiometer

    // Zero the potentiometer
    void zero(void);                            ///< The method to zero the potentiometer to its current position
};

#endif // _POT_