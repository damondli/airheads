/** @file ultrasonic.h
 *  @brief The header file for an HC_SR04 Ultrasonic Sensor class
 * 
 *  @author Damond Li
 *  @author Arielle Sampson
 *  @date 2022-Nov-30 Original file
 */

// Compile the header file only once
#ifndef ULTRASONIC
#define ULTRASONIC

#include <Arduino.h>

/** @brief  Class for an HC_SR04 Ultrasonic Sensor
 */
class Ultrasonic //This class operates an HC_SR04 Ultrasonic Sensor 
{
protected:
    float distance;         ///< The distance between the ultrasonic sensor and the object in front of it
    uint8_t echoPin;        ///< The GPIO echo pin used to measure the time between ultrasonic pulses 
    uint8_t trigPin;        ///< The GPIO trigger pin that sends out ultrasonic pulses
    long duration;          ///< The time between received ultrasonic pulses


public:
    Ultrasonic (uint8_t echoPin, uint8_t trigPin);          ///< Constructor for the ultrasonic sensor class
    float get_distance (void);                              ///< The method to get the distance measured from the ultrasonic sensor
};

#endif // ULTRASONIC