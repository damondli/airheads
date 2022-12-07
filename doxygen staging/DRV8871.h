/** @file DRV8871.h
 *  @brief Header file for the DRV8871 motor driver. This contains the
 *         class for the motor driver which is responsible for generating
 *         the appropriate PWM wave for each instance of the motor object. 
 * 
 * @author  Damond Li
 * @date    2022-Oct-27 Original file
 */

// Compile this header file only once
#ifndef _DRV8871_H_
#define _DRV8871_H_

#include <Arduino.h>

/** @brief  Class for a motor driver using the DRV8871 chip. Primarily
 * responsible for setting the appropriate PWM signal for an H-bridge
 * motor driver chip.
 */
class DRV8871
{
protected:
    // Variables for declaring each motor
    uint8_t PIN_A;                      ///< PIN_A is the GPIO pin to output a PWM wave for a positive duty cycle
    uint8_t PIN_B;                      ///< PIN_B is the GPIO pin to output a PWM wave for a negative duty cycle
    uint8_t CHANNEL_A;                  ///< The timing channel for PIN_A
    uint8_t CHANNEL_B;                  ///< The timing channel for PIN_B

    // PWM properties
    const uint16_t frequency = 20000;                           ///< The frequency of the PWM wave
    const uint8_t resolution = 8;                               ///< The resolution of the PWM wave [bits]
    const uint8_t max_duty = uint8_t (pow(2, resolution) - 1);  ///< The highest value describing a 100% duty cycle as a function of the resolution

public:
    DRV8871(uint8_t pin_A, uint8_t pin_B, uint8_t channel_A, uint8_t channel_B);    ///< Constructor for the DRV8871 class

    int16_t duty;                   ///< The duty cycle to operate the motor
    void set_duty (int16_t);        ///< The method to set the appropriate duty cycle
};

#endif // _DRV8871_H_