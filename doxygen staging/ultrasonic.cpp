/** @file ultrasonic.cpp
 *  @brief The source file for an HC_SR04 Ultrasonic Sensor class
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

#include <Arduino.h>
#include "ultrasonic.h"

/** @brief   Constructor which creates an ultrasonic sensor object
 *  @param   echo The GPIO pin used to measure the time between ultrasonic pulses
 *  @param   trig The GPIO pin used to send out ultrasonic pulses
 */
Ultrasonic::Ultrasonic(uint8_t echo, uint8_t trig)
{
    // Establish the trigger and echo pins
    trigPin = trig;
    echoPin = echo;

    // Set the pins accordingly
    pinMode(trigPin, OUTPUT);   // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT);    // Sets the echoPin as an INPUT
}

/** @brief   Measure the distance between the sensor and the object in front of it
 *  @returns The distance, in centimeters, between the sensor and the object in front of it
 */
float Ultrasonic::get_distance (void)
{
    distance;       // variable to store calculated distance
    duration;       // variable for the duration of sound wave travel
          
    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
            
    return distance; // return distance measurement in cm
}