/** @file ultrasonic.cpp
 *  This Arduino Library operates an HC-SR04 Ultrasonic Sensor
 *  @author Arielle Sampson and Damond Li
 *  @date 2022-Nov-03 
 *  @copyright 
 */

#include <Arduino.h>
#include "ultrasonic.h"

ultrasonic::ultrasonic(uint8_t echo, uint8_t trig)
{
    trigPin = trig;
    echoPin = echo;
}

float ultrasonic::get_distance (void)
{
    float echoTime; // define variable to store time it takes for a ping
                    // to bounce off an object
    float distance; // variable to store calculated distance
    long duration;  // variable for the duration of sound wave travel
          
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