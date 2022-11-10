/** @file control_surfaces.cpp
 * This file provides angle control for the rudder and elevator
 * of a model airplane. This file controls two DC motors in 
 * microservo bodies that move the rudder and elevator control
 * surfaces. 
 * 
 *  @author ME 507 Airheads
 *  @date   2022-Nov-09
 */

#include <Arduino.h>

int analogInPin = A0;
int sensorValue = 0;
int outputValue = 0;
int transistorPin = 3;

int danger_pitch = 61; // dangerous pitch angle
 
/** @brief   Task which moves the elevator of a model airplane.
 *  @details This task moves a DC motor in microservo body which
 *             moves the elevator of a model airplane, changing
 *              the airplane's pitch during flight.
 *  @param   p_params 
 */
void task_elevator (void* p_params)
{
    for (;;)
    {
        
        vTaskDelay(10); // runs every 10 ms
    }
}

/** @brief   Task which moves the rudder of a model airplane.
 *  @details This task moves a DC motor in microservo body which
 *             moves the rudder of a model airplane, changing
 *              the airplane's yaw during flight.
 *  @param   p_params 
 */
void task_rudder (void* p_params)
{
    for (;;)
    {
        
        vTaskDelay(10); // runs every 10 ms
    }
}

/** @brief   The Arduino setup function.
 *  @details This function is used to set up the microcontroller by starting
 *           the serial port and creating the assigned tasks.
 */
void setup()
{
    Serial.begin(9600);

    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(transistorPin, OUTPUT);
}

/** @brief   The Arduino loop function. Empty for this assignment.
 *  @details This function is called periodically by the Arduino system. It
 *           runs as a low priority task. On some microcontrollers it will
 *           crash when FreeRTOS is running, so we usually don't use this
 *           function for anything, instead just having it delay itself. 
 */
void loop() 
{
sensorValue = analogRead(analogInPin)/4;
outputValue = map(sensorValue, 0, 1023, 0, 255);
analogWrite(transistorPin, sensorValue);

    if (sensorValue >= 160)
    {
        digitalWrite(8, HIGH);
        digitalWrite(9, LOW);
    }

    else
    { 
        digitalWrite(9, HIGH);
        digitalWrite(8, LOW);
    }

    delay(10);   

 }
 

//References 
// https://learn.parallax.com/tutorials/robot/shield-bot/robotics-board-education-shield-arduino/chapter-2-shield-lights-servo-14
//https://create.arduino.cc/projecthub/RiddledExistence/controlling-a-servo-motor-with-thumb-joystick-46a4d3
//https://circuitdigest.com/microcontroller-projects/dc-motor-speed-control-using-arduino-and-potentiometer
//https://www.instructables.com/Arduino-DC-motor-speed-control-potentiometer/