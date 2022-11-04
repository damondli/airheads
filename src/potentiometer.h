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

class potentiometer
{
protected:
    uint8_t ADC_PIN;

public:
    potentiometer(uint8_t pin);

    uint8_t adc;

    float get_position(void);
};

#endif // _POT_