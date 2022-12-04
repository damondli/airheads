/** @file ultrasonic.h
 *  The header file for an HC_SR04 Ultrasonic Sensor class
 * 
 *  Based on an examples by Arbi Abdul Jabbaar at 
 *  @c https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6
 * 
 *  @author Arbi Abdul Jabbaar
 *  @author Damond Li
 *  @author Arielle Sampson
 *  @date 2019-Sept-17 Original file
 *  @date 2022-Nov-30 Modified for Airheads Glider Project use by Li and Sampson
 *  @copyright 2019 by the author
 */

// Compile the header file only once
#ifndef ULTRASONIC
#define ULTRASONIC

#include <Arduino.h>

/** @brief  Class for an HC_SR04 Ultrasonic Sensor
 */
class ultrasonic //This class operates an HC_SR04 Ultrasonic Sensor 
{
protected:
    float distance;         ///< The distance between the ultrasonic sensor and the object in front of it
    uint8_t echoPin;        ///< The GPIO echo pin used to measure the time between ultrasonic pulses 
    uint8_t trigPin;        ///< The GPIO trigger pin that sends out ultrasonic pulses
    long duration;          ///< The time between received ultrasonic pulses


public:
    ultrasonic (uint8_t echoPin, uint8_t trigPin);          ///< Constructor for the ultrasonic sensor class
    float get_distance (void);                              ///< The method to get the distance measured from the ultrasonic sensor
};

#endif // ULTRASONIC