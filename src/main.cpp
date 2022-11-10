/** @file main.cpp
 *  @brief Main file for ME 507 Airheads project. Safely lands an unpowered model airplane.
 *  @details Includes ultrasonic sensor, potentiometer, motor driver, and flight controller tasks.
 *  @author ME 507 Airheads
 *  @date 2022-Nov-10 
 */

#include <Arduino.h>
#include "shares.h"
#include "taskshare.h"
#include "taskqueue.h"
#include "PrintStream.h"

// Modules
#include "DRV8871.h"
#include "ultrasonic.h"
#include "potentiometer.h"
#include "PIDController.h"

// Shares

Share<bool> near_ground ("Near Ground");
Share<int16_t> rudder_duty ("Rudder motor duty cycle");
Share<int16_t> elev_duty ("Elevator motor duty cycle");


// Rudder Motor
#define RUDDER_PIN_A 16
#define RUDDER_PIN_B 13
#define RUDDER_CHANNEL_A 0
#define RUDDER_CHANNEL_B 1

// Elevator Motor
#define ELEVATOR_PIN_A 16
#define ELEVATOR_PIN_B 13
#define ELEVATOR_CHANNEL_A 0
#define ELEVATOR_CHANNEL_B 1

// Ultrasonic
#define TRIG 2
#define ECHO 3

/** @brief   Ultrasonic sensor measures distance to the ground
 *  @details Ultrasonic sensor mounted on the airplane measures the 
 *           distance from the airplane to the ground. When the airplane
 *           is within 1 foot of the ground, the airplane's control 
 *           surfaces will move into "landing configuration" where the 
 *           pitch will be x degrees up for a soft landing.
 *  @param   p_params A pointer to parameters passed to this task. This 
 *           pointer is ignored; it should be set to @c NULL in the 
 *           call to @c xTaskCreate() which starts this task
 */
void task_ultrasonic (void* p_params)
{
    Serial << "Ultrasonic Sensor Task Begin" << endl;

    // Task period
    const uint8_t period = 1;
    // Near ground boolean
    near_ground.put(true);
    // Timer
    uint8_t counter = 0;
    // Distance
    float distance;
    // Height and timer threshold
    const uint8_t threshold = 20;               // cm
    const uint8_t time_buffer = period * 20;    // ms

    // Create object
    ultrasonic ultra = ultrasonic(ECHO, TRIG);

    while (true)
    {
      // Get the distance from the sensor
      distance = ultra.get_distance();
      // If the distance is below height threshold, start counting
      // Stop counting when counter exceeds 10 seconds to prevent overflow
      if (distance < threshold && counter < 10000)
      {
        counter++;
      }
      else if (distance > threshold)
      {
        counter = 0;
      }
      
      // Swap boolean to true if below threshold for long enough
      if (counter > time_buffer)
      {
        near_ground.put(true);
      }
      else
      {
        near_ground.put(false);
      }
      vTaskDelay(period);
    }
}


/** @brief   Controller for both rudder and elevator control surfaces
 *  @details Retrieves IMU, potentiometer, and ultrasonic sensor data and writes 
 *           motor duty cycles to shares. The motor tasks use the duty cycles
 *           to move the rudder and elevator control surfaces.
 *  @param   p_params An unused pointer to (no) parameters passed to this task
 */
void task_controller (void* p_params)
{
    Serial << "Controller Task Begin" << endl;
  
    const uint8_t TASK_CONTROLLER_PERIOD = 10;  // Period of controller task (ms)

    // Controller objects
    PIDController yaw2rudder =    // Controller for rudder angle based on yaw
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD); 
    PIDController rudder2duty =   // Controller for duty cycle based on rudder angle
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD);
    PIDController pitch2elev =    // Controller for elevator angle based on pitch
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD);
    PIDController elev2duty =     // Controller for duty cycle based on elevator angle
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD);

    // Initialize variables
    float yawC;           // Current yaw (deg)
    float yawD;           // Desired yaw (deg)
    float pitchC;         // Current pitch (deg)
    float pitchD;         // Desired pitch (deg)

    float rudderAngleD;   // Desired rudder angle (deg)
    float rudderAngleC;   // Current rudder angle (deg)
    float elevAngleD;     // Desired elevator angle (deg)
    float elevAngleC;     // Current elevator angle (deg)
    
    while (true) 
    {
        // Check whether the glider is near ground
        if (near_ground.get()) 
        {
            pitchD = 5;
        }
        else 
        {
            pitchD = 0;
        }

        yawD = 0;

        // Get current values
        yawC = /* get yaw from IMU*/ 0;
        pitchC = /* get pitch from IMU*/ 0;
    
        // Calculate desired control surface angles
        rudderAngleD = yaw2rudder.getCtrlOutput(yawC,yawD);
        rudder_duty.put(rudder2duty.getCtrlOutput(rudderAngleC,rudderAngleD));
        
        elevAngleD = pitch2elev.getCtrlOutput(pitchC,pitchD);
        elev_duty.put(elev2duty.getCtrlOutput(elevAngleC,elevAngleD));
        
        vTaskDelay(TASK_CONTROLLER_PERIOD);
    }
}

/** @brief   
 *  @details 
 *  @param   p_params A pointer to parameters passed to this task. This 
 *           pointer is ignored; it should be set to @c NULL in the 
 *           call to @c xTaskCreate() which starts this task
 */
void task_rudder_motor (void* p_params)
{
    // Motor task period
    const uint8_t period = 10;

    Serial << "Rudder Motor Task Begin" << endl;
    // Create object
    DRV8871 rudder = DRV8871(RUDDER_PIN_A, RUDDER_PIN_B, RUDDER_CHANNEL_A, RUDDER_CHANNEL_B);

    while (true)
    {
      rudder.set_duty(rudder_duty.get());
      vTaskDelay(period);
    }
}

/** @brief   
 *  @details 
 *  @param   p_params A pointer to parameters passed to this task. This 
 *           pointer is ignored; it should be set to @c NULL in the 
 *           call to @c xTaskCreate() which starts this task
 */
void task_elevator_motor (void* p_params)
{
    // Motor task period
    const uint8_t period = 10;

    Serial << "Elevator Motor Task Begin" << endl;
    // Create object
    DRV8871 rudder = DRV8871(
        ELEVATOR_PIN_A, ELEVATOR_PIN_B, ELEVATOR_CHANNEL_A, ELEVATOR_CHANNEL_B);

    while (true)
    {
      rudder.set_duty(elev_duty.get());
      vTaskDelay(period);
    }
}


/** @brief   The Arduino setup function.
 *  @details This function is used to set up the microcontroller by starting
 *           the serial port and creating the tasks.
 */
void setup (void) 
{
    // The serial port must begin before it may be used
    Serial.begin (115200);

    // Wait for serial to finish setting up before proceeding
    while (!Serial) 
    {
    }

    Serial << "Serial is ready. " << endl;

  
    // Create the task which outputs the high frequency square wave.
    // xTaskCreate (task_motor_A, "Motor A", 2048, NULL, 4, NULL);
    
    // Create the task which outputs the low frequency square wave.
    // xTaskCreate (task_motor_B, "Motor B", 2048, NULL, 3, NULL);

    // Task for the potentiometer testing
    xTaskCreate (task_rudder_motor, "Rudder Motor", 2048, NULL, 3, NULL);

    // Task for the potentiometer testing
    xTaskCreate (task_elevator_motor, "Elevator Motor", 2048, NULL, 3, NULL);
    
    // Task for the ultrasonic sensor
    xTaskCreate (task_ultrasonic, "Ultrasonic Sensor", 2048, NULL, 3, NULL);

    // Task for the flight surface controls (rudder and elevator)
    xTaskCreate (task_controller, "Flight Controls", 2048, NULL, 3, NULL);
}


/** @brief   The Arduino loop function.
 *  @details This function is called periodically by the Arduino system. It
 *           runs as a low priority task. On some microcontrollers it will
 *           crash when FreeRTOS is running, so we usually don't use this
 *           function for anything. 
 */
void loop (void)
{
}