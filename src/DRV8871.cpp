/** @file DRV8841.cpp
 * 
 */


#include <Arduino.h>
#include "DRV8871.h"

DRV8871::DRV8871(uint8_t pin_A, uint8_t pin_B)
{
    // Establish the out pins
    uint8_t PIN_A = pin_A;
    uint8_t PIN_B = pin_B;

    // Set the pin modes
    pinMode (PIN_A, OUTPUT);
    pinMode (PIN_B, OUTPUT);
}   

void DRV8871::enable(void)
{
    digitalWrite (PIN_A, LOW);
    digitalWrite (PIN_B, LOW);
}

void disable(void)
{
    digitalWrite (PIN_A, LOW);
    digitalWrite (PIN_B, LOW);
}

void set_duty(float duty_cycle)
{

}