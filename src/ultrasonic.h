/** @file ultrasonic.h
 *  This Arduino Library operates an HC-SR04 Ultrasonic Sensor
 *  @author Arielle Sampson and Damond Li
 *  @date 2022-Nov-03 
 *  @copyright 
 */

#ifndef ULTRASONIC
#define ULTRASONIC

#include <Arduino.h>

class ultrasonic //This class operates an HC_SR04 Ultrasonic Sensor 
{
protected:
    // uint8_t ultrasonic_address = 0x36;
    float echoTime;
    
    float distance;

    uint8_t echoPin;
    uint8_t trigPin;


public: 
    long duration;
    ultrasonic (uint8_t echoPin, uint8_t trigPin);

    float get_distance (void);
};

#endif // ULTRASONIC