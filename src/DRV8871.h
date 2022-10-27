/** motor.h
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
    // variables
    const uint8_t PIN_A;
    const uint8_t PIN_B;

public:
    DRV8871(uint8_t, uint8_t);

    void enable (void);
    void disable (void);
    void set_duty (float);
};

#endif // _DRV8871_H_