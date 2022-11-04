/** @file potentiometer.cpp
 * 
 */

#include <Arduino.h>
#include "potentiometer.h"

potentiometer::potentiometer(uint8_t pin, float offset)
{
    // Establish the pin that will read the voltage
    ADC_PIN = pin;
    voltage_offset = offset;
}

float potentiometer::get_voltage(void)
{
    // Default resolution is 12 bits. Outputs 0 - 4096
    adc_value = analogRead(ADC_PIN);

    // Convert the ADC values to a voltage
    voltage = VOLTAGE_SOURCE * adc_value / ADC_RANGE;

    return voltage;
}

float potentiometer::get_angle(void)
{
    // Default resolution is 12 bits. Outputs 0 - 4096
    adc_value = analogRead(ADC_PIN);

    // Convert the ADC values to a voltage
    voltage = VOLTAGE_SOURCE * adc_value / ADC_RANGE;

    // TODO
    // Modify the ADC value or voltage value to the desired
    // output: angle, position, or voltage.
    float angle = (voltage - voltage_offset) * VOLTAGE_TO_DEGREES;

    return angle;
}

void potentiometer::zero(void)
{  
    // Assuming the potentiometer is positioned at the zero
    // Get current voltage reading
    float current_voltage = get_voltage();

    // Set the offset to the current voltage
    voltage_offset = current_voltage;
}