/** @file main.cpp
 *  @brief Main file for ME 507 Airheads project. Safely lands an unpowered model airplane.
 *  @details Includes ultrasonic sensor, potentiometer, motor driver, and flight controller tasks.
 *  @author ME 507 Airheads
 *  @date 2022-Nov-10 
 */

#include <Arduino.h>
#include "shares.h"
#include "taskshare.h"
//#include "taskqueue.h"
#include "PrintStream.h"

// Modules
#include "DRV8871.h"
#include "ultrasonic.h"
#include "potentiometer.h"
#include "PIDController.h"

// Shares

Share<bool> near_ground ("Near Ground");
Share<uint8_t> tc_state ("Task Controller State");
Share<int16_t> rudder_duty ("Rudder motor duty cycle");
Share<int16_t> elev_duty ("Elevator motor duty cycle");
Share<float> yawC ("Current yaw from IMU");
Share<float> pitchC ("Current pitch from IMU");



// Rudder Motor (Motor 1)
#define RUDDER_PIN_IN1   15  // 15
#define RUDDER_PIN_IN2   32  // 32
#define RUDDER_CHANNEL_A 0
#define RUDDER_CHANNEL_B 1

// Elevator Motor (Motor 0)
#define ELEVATOR_PIN_IN1   27  // 27
#define ELEVATOR_PIN_IN2   33  // 33
#define ELEVATOR_CHANNEL_A 2
#define ELEVATOR_CHANNEL_B 3

// Potentiometers
#define ELEVATOR_POT_PIN 26
#define RUDDER_POT_PIN   25

// Ultrasonic
#define TRIG 12
#define ECHO 13

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
    const uint8_t period = 100;
    // Near ground boolean
    //near_ground.put(true);

    // Distance
    float distance;
    // Height and timer threshold
    const uint8_t threshold = 20;               // cm
    const uint8_t time_buffer = period * 20;    // ms

    // Create object
    Serial.println("Constructing the ultrasonic object");
    ultrasonic ultra = ultrasonic(ECHO, TRIG);

    while (true)
    {
        // Get the distance from the sensor
        distance = ultra.get_distance();
        
        // If the distance is below height threshold, start counting
        // Stop counting when counter exceeds 10 seconds to prevent overflow
        near_ground.put(distance < threshold);
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
    PIDController yaw2rudder =      // Controller for rudder angle based on yaw
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD); 
    PIDController rudder2duty =     // Controller for duty cycle based on rudder angle
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD);
    PIDController pitch2elev =      // Controller for elevator angle based on pitch
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD);
    PIDController elev2duty =       // Controller for duty cycle based on elevator angle
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD);

    // Create potentiometer object and zero the current reading
    Potentiometer rudderPot = Potentiometer(RUDDER_POT_PIN, 0);
    rudderPot.zero();

    // Create potentiometer object and zero the current reading
    Potentiometer elevPot = Potentiometer(ELEVATOR_POT_PIN, 0);
    elevPot.zero();

    // Initialize variables
    float yawD;                     // Desired yaw (deg)
    float pitchD;                   // Desired pitch (deg)   

    float rudderAngleD;             // Desired rudder angle (deg)
    float rudderAngleC;             // Current rudder angle (deg)
    float rudderAngleMin = -120;    // Minimum allowable rudder angle (deg)
    float rudderAngleMax = 120;     // Maximum allowable rudder angle (deg)

    float elevAngleD;               // Desired elevator angle (deg)
    float elevAngleC;               // Current elevator angle (deg)
    float elevAngleMin = -120;      // Minimum allowable elevator angle (deg)
    float elevAngleMax = 120;       // Maximum allowable elevator angle (deg)

    float rudderDutyD;              // Rudder motor duty cycle (-100% to 100% incl.)
    float elevDutyD;                // Elev motor duty cycle (-100% to 100% incl.)

    uint16_t delay_time = 0;        // Current amount of time (ms) in inactive delay
    tc_state.put(0);

    
    while (true) 
    {
        if (tc_state.get() == 0)          // STATE 0: CHECK FOR LAUNCH
        {        
            // Passive state waiting for external callback to switch state
            Serial.print(" tc_state is 0 ");

            tc_state.put(1);

        }
        else if (tc_state.get() == 1)     // STATE 1: KEEP INACTIVE FOR 1 SECOND
        {

            Serial.print(" 1 ");

            // Add one task period to accumulated delay time (ms)
            if (near_ground.get() == 0) 
            {
                delay_time += TASK_CONTROLLER_PERIOD;         
            }  
            else
            {
                delay_time = 0;
            }

            // If total delay time has reached 1000 ms...
            if (delay_time >= 2000) 
            {
                Serial.print("1");

                tc_state.put(2);       // Move to active state
                delay_time = 0;     // Reset counter
            }

        }
        else if (tc_state.get() == 2)     // STATE 2: CONTROLLER ACTIVE
        {

            Serial.print(" 2 ");

            if (near_ground.get() == 1) 
            {
                delay_time += TASK_CONTROLLER_PERIOD;         
            }  
            else
            {
                delay_time = 0;
            }

            // If total delay time has reached 1000 ms...
            if (delay_time >= 2000) 
            {
                tc_state.put(0);       // Move to active state
                delay_time = 0;     // Reset counter
            }



            
            // Check whether the glider is near ground
            if (near_ground.get()) 
            {
                pitchD = 5;   // NEEDS TUNING
            }
            else 
            {
                pitchD = 0;   // NEEDS TUNING
            }

            yawD = 0;    

            yawC.put(0);
            pitchC.put(0);     
        
            // Calculate desired rudder angle and then saturate
            rudderAngleD = yaw2rudder.getCtrlOutput(yawC.get(),yawD);
            if (rudderAngleD > rudderAngleMax) 
            {
                rudderAngleD = rudderAngleMax;
            }
            else if (rudderAngleD < rudderAngleMin) 
            {
                rudderAngleD = rudderAngleMin;
            }

            // Get current rudder angle
            rudderAngleC = rudderPot.get_angle();
            Serial.print(rudderAngleC);
            Serial.print(" ");

            // Calculate desired rudder motor duty cycle, saturate, then put to share
            rudderDutyD = rudder2duty.getCtrlOutput(rudderAngleC,rudderAngleD);
            if (rudderDutyD > 100) 
            {
                rudder_duty.put(100);
            }
            else if (rudderDutyD < -100) 
            {
                rudder_duty.put(-100);
            }
            else
            {
                rudder_duty.put((int16_t) rudderDutyD);
            }
            

            // Calculate desired elevator angle and then saturate
            elevAngleD = pitch2elev.getCtrlOutput(pitchC.get(),pitchD);
            if (elevAngleD > elevAngleMax)
            {
                elevAngleD = elevAngleMax;
            }
            else if (elevAngleD < elevAngleMin)
            {
                elevAngleD = elevAngleMin;
            }

            // Get current elevator angle
            elevAngleC = elevPot.get_angle();
            Serial.println(elevAngleC);

            // Calculate desired elevator motor duty cycle, saturate, then put to share
            elevDutyD = elev2duty.getCtrlOutput(elevAngleC,elevAngleD);
            if (elevDutyD > 100) 
            {
                elev_duty.put(100);
            }
            else if (elevDutyD < -100) 
            {
                elev_duty.put(-100);
            }
            else 
            {
                elev_duty.put((int16_t) round(elevDutyD));
            }

        }

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
    DRV8871 rudder = DRV8871(RUDDER_PIN_IN1, RUDDER_PIN_IN2, RUDDER_CHANNEL_A, RUDDER_CHANNEL_B);

    while (true)
    {
      rudder.set_duty(rudder_duty.get());
      vTaskDelay(period);
    }
    

    // The following is code used for debugging and demo

    /*
    // Initialize variables
    float rudderAngleD;               // Desired elevator angle (deg)
    float rudderAngleC;               // Current elevator angle (deg)
    float rudderDutyD;                // Elev motor duty cycle (-100% to 100% incl.)

    // Motor task period
    const uint8_t PERIOD = 10;
    char key;
    const int8_t ANGLE = 50;
    
    // Initialize PID contoller
    PIDController rudder2duty =       // Controller for duty cycle based on elevator angle
        PIDController(1,0,0, PERIOD);

    // Create motor object
    DRV8871 rudder = DRV8871(
        RUDDER_PIN_IN1, RUDDER_PIN_IN2, RUDDER_CHANNEL_A, RUDDER_CHANNEL_B);

    // Create potentiometer object and zero the current reading
    Potentiometer rudderPot = Potentiometer(13, 0);
    rudderPot.zero();

    while (true)
    {
        rudderAngleC = rudderPot.get_angle();
        // Check if there are any keys pressed
        if (Serial.available() > 0)
        {
            key = Serial.read();
            if (key == 'a')
            {
                rudderDutyD = rudder2duty.getCtrlOutput(rudderAngleC, ANGLE);
            }
            else if (key == 'd')
            {
                rudderDutyD = rudder2duty.getCtrlOutput(rudderAngleC, -ANGLE);
            }
            else if (key == 's')
            {
                rudderDutyD = rudder2duty.getCtrlOutput(rudderAngleC, 0);
            }
        }

        rudder.set_duty(rudderDutyD);
        vTaskDelay(PERIOD);   
    }
    */
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
        ELEVATOR_PIN_IN1, ELEVATOR_PIN_IN2, ELEVATOR_CHANNEL_A, ELEVATOR_CHANNEL_B);

    while (true)
    {
      rudder.set_duty(elev_duty.get());
      vTaskDelay(period);
    }
    

    // The following is code used for debugging and demo

    /*
    // Initialize variables
    float elevAngleD;               // Desired elevator angle (deg)
    float elevAngleC;               // Current elevator angle (deg)
    float elevDutyD;                // Elev motor duty cycle (-100% to 100% incl.)

    // Motor task period
    const uint8_t PERIOD = 10;
    char key;
    const int8_t ANGLE = 50;
    
    // Initialize PID contoller
    PIDController elev2duty =       // Controller for duty cycle based on elevator angle
        PIDController(1,0,0, PERIOD);

    // Create motor object
    DRV8871 elevator = DRV8871(
        ELEVATOR_PIN_IN1, ELEVATOR_PIN_IN2, ELEVATOR_CHANNEL_A, ELEVATOR_CHANNEL_B);

    // Create potentiometer object and zero the current reading
    Potentiometer elevPot = Potentiometer(12, 0);
    elevPot.zero();

    while (true)
    {
        elevAngleC = elevPot.get_angle();
        // Check if there are any keys pressed
        if (Serial.available() > 0)
        {
            key = Serial.read();
            if (key == '8')
            {
                elevDutyD = elev2duty.getCtrlOutput(elevAngleC, ANGLE);
            }
            else if (key == '2')
            {
                elevDutyD = elev2duty.getCtrlOutput(elevAngleC, -ANGLE);
            }
            else if (key == '5')
            {
                elevDutyD = elev2duty.getCtrlOutput(elevAngleC, 0);
            }
        }

        elevator.set_duty(elevDutyD);
        vTaskDelay(PERIOD);
    }

    */
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
    // xTaskCreate (task_rudder_motor, "Rudder Motor", 2048, NULL, 3, NULL);

    // Task for the potentiometer testing
    // xTaskCreate (task_elevator_motor, "Elevator Motor", 2048, NULL, 3, NULL);
    
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