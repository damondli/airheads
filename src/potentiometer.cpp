/** @file potentiometer.cpp
 * This is the source file containing the constructor and methods for
 * a generic potentiometer class.
 * 
 * @author  Damond Li
 * @date    2022-Nov-03 Original file
 */

#include <Arduino.h>
#include "potentiometer.h"

/** @brief   Constructor which creates a potentiometer object
 *  @param   pin The GPIO input pin to read voltages from
 *  @param   offset The offset values used to zero the potentiometer
 */
Potentiometer::Potentiometer(uint8_t pin, float offset)
{
    // Establish the pin that will read the voltage
    ADC_PIN = pin;
    // Establish the voltage offset
    voltage_offset = offset;
}

/** @brief   Measures the voltage at the input pin
 *  @returns The voltage measured at the input pin
 */
float Potentiometer::get_voltage(void)
{
    // Default resolution is 12 bits. Outputs 0 - 4096
    adc_value = analogRead(ADC_PIN);

    // Convert the ADC values to a voltage
    voltage = VOLTAGE_SOURCE * adc_value / ADC_RANGE;

    return voltage;
}

/** @brief   Measures position of the potentiometer
 *  @returns The position of the potentiometer in units of degrees
 */
float Potentiometer::get_angle(void)
{
    // Default resolution is 12 bits. Outputs 0 - 4096
    adc_value = analogRead(ADC_PIN);

    // Convert the ADC values to a voltage
    voltage = VOLTAGE_SOURCE * adc_value / ADC_RANGE;

    // Offset the voltage reading and convert to degrees
    float angle = (voltage - voltage_offset) * VOLTAGE_TO_DEGREES;

    return angle;
}

/** @brief   Zeros the position of the potentiometer by setting the offset voltage
 *           to the voltage measured at its current position
 */
void Potentiometer::zero(void)
{  
    // Assuming the potentiometer is positioned at the zero
    // Get current voltage reading
    float current_voltage = get_voltage();

    // Set the offset to the current voltage
    voltage_offset = current_voltage;
}