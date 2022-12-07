/** @file DRV8871.cpp
 *  @brief source file for a DRV8871 motor driver. This contains the methods that
 *         construct an instance of the DRV8871 motor object and set the desired duty cycle.
 * 
 *  Based on an examples by A. Sinha at 
 *  @c https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
 * 
 *  @author  Rui Santos
 *  @author  Sara Santos
 *  @author  Damond Li
 *  @date    2018-Oct Original file
 *  @date    2022-Oct-27 Modified for a motor driver class by Li
 *  @copyright 2018 by the authors
 */

#include <Arduino.h>
#include "DRV8871.h"

/** @brief   Constructor for the DRV8871 motor driver class
 *  @param   pin_A The GPIO pin from the ESP32 (non-zero PWM for a positive duty cycle)
 *  @param   pin_B The GPIO pin from the ESP32 (non-zero PWM for a negative duty cycle)
 *  @param   channel_A The timing channel for pin_A
 *  @param   channel_B The timing channel for pin_B
 */
DRV8871::DRV8871(uint8_t pin_A, uint8_t pin_B, uint8_t channel_A, uint8_t channel_B)
{
    // Establish the output pins
    PIN_A = pin_A;
    PIN_B = pin_B;

    // Establish the channels
    CHANNEL_A = channel_A;
    CHANNEL_B = channel_B;


    // Setup pins with the appropriate resolution and frequency
    ledcSetup(CHANNEL_A, frequency, resolution);
    ledcSetup(CHANNEL_B, frequency, resolution);

    // Attach the pins to the channel
    ledcAttachPin(PIN_A, CHANNEL_A);
    ledcAttachPin(PIN_B, CHANNEL_B);
}   

/** @brief   Outputs the desired PWM signal to the appropriate output pin given a duty cycle
 *  @param   duty_cycle The duty cycle to run the motors
 */
void DRV8871::set_duty(int16_t duty_cycle)
{
    // Check max and min boundaries for duty cycle inputs
    if (duty_cycle > 100)
    {
        duty = 100;
    }
    else if (duty_cycle < -100)
    {
        duty = -100;
    }
    else
    {
        duty = duty_cycle;
    }

    // Scale the duty cycle according to the resolution of the channel
    duty = duty * max_duty / 100;

    // Check to see which channel to use given the signage of the duty cycle
    if (duty > 0)                           // Use Channel A
    {
        ledcWrite(CHANNEL_A, duty);
        ledcWrite(CHANNEL_B, 0);
    }
    else if (duty < 0)                      // Use Channel B
    {
        ledcWrite(CHANNEL_B, (-1 * duty));
        ledcWrite(CHANNEL_A, 0);
    }
    else                                    // Both channels set to zero duty cycle
    {
        ledcWrite(CHANNEL_A, 0);
        ledcWrite(CHANNEL_B, 0);
    }
}