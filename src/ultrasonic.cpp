/** @file ultrasonic.cpp
 *  This Arduino Library operates an HC-SR04 Ultrasonic Sensor
 *  @author Arielle Sampson and Damond Li
 *  @date 2022-Nov-03 
 *  @copyright 
 */

#include <Arduino.h>
#include "ultrasonic.h"
#include "taskshare.h"
#include <Wire.h>

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
    long duration; // variable for the duration of sound wave travel
          
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


/** @brief   The Arduino setup function.
 *  @details This function is used to set up the microcontroller by starting
 *           the serial port and creating the assigned tasks.
 */
void setup(void)
{
    Serial.begin (115200); // The serial port must begin before it may be used
    
    uint8_t trigPin = 12;
    uint8_t echoPin = 13;

    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

    ultrasonic sensor = ultrasonic(echoPin, trigPin);
}


/** @brief   The Arduino loop function. Empty for this assignment.
 *  @details This function is called periodically by the Arduino system. It
 *           runs as a low priority task. On some microcontrollers it will
 *           crash when FreeRTOS is running, so we usually don't use this
 *           function for anything, instead just having it delay itself. 
 */
// void loop (void)
// {
//     distance = get_distance(); // variable to store the distance measured by the sensor

//     // Displays the distance on the Serial Monitor
//     Serial.print("Distance: ");
//     Serial.print(distance);
//     Serial.println(" cm");
//     vTaskDelay (10);
// }
