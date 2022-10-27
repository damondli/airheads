/** @file DRV8841.cpp
 * 
 */


#include <Arduino.h>
#include "DRV8871.h"
#include "PrintStream.h"

DRV8871::DRV8871(uint8_t pin_A, uint8_t pin_B, uint8_t channel_A, uint8_t channel_B)
{
    // Establish the out pins
    uint8_t PIN_A = pin_A;
    uint8_t PIN_B = pin_B;

    uint8_t CHANNEL_A = channel_A;
    uint8_t CHANNEL_B = channel_B;

    // Setup pins
    ledcSetup(CHANNEL_A, frequency, resolution);
    ledcSetup(CHANNEL_B, frequency, resolution);

    // Attach the pins to the channel
    ledcAttachPin(PIN_A, CHANNEL_A);
    ledcAttachPin(PIN_B, CHANNEL_B);
}   

void DRV8871::enable(void)
{
    /*
    digitalWrite (PIN_A, LOW);
    digitalWrite (PIN_B, LOW);
    */
}

void DRV8871::disable(void)
{
    ledcWrite(CHANNEL_A, 0);
    ledcWrite(CHANNEL_B, 0);
}

void DRV8871::set_duty(uint8_t duty_cycle)
{
    // Check max and min boundaries for duty cycle inputs
    // uint8_t duty;
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

    duty = duty * max_duty / 100;

    if (duty > 0) // Use Channel A
    {
        ledcWrite(CHANNEL_A, duty);
        ledcWrite(CHANNEL_B, 0);
    }
    else if (duty < 0) // Use Channel B
    {
        ledcWrite(CHANNEL_B, (-1 * duty));
        ledcWrite(CHANNEL_A, 0);
    }
    else
    {
        ledcWrite(CHANNEL_A, 0);
        ledcWrite(CHANNEL_B, 0);
    }
}