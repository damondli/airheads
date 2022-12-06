/** @file main.cpp
 *  @brief Main file for ME 507 Airheads project. Safely lands an unpowered model airplane.
 *  @details Includes ultrasonic sensor, potentiometer, motor driver, and flight controller tasks.
 *  @author ME 507 Airheads
 *  @date 2022-Nov-10 
 */

#include <Arduino.h>
#include "shares.h"
#include "taskshare.h"
#include "PrintStream.h"
#include <time.h>
#include <network.h>

// Modules
#include "DRV8871.h"
#include "ultrasonic.h"
#include "potentiometer.h"
#include "PIDController.h"
#include "IMU.h"

// Shares
Share<bool> near_ground ("Near Ground");                    ///< A share boolean that reads true if the glider is near ground
Share<uint8_t> tc_state ("Task Controller State");          ///< A share integer for finite state machine
Share<int16_t> rudder_duty ("Rudder motor duty cycle");     ///< A share containing the duty cycle for rudder motor
Share<int16_t> elev_duty ("Elevator motor duty cycle");     ///< A share containing the duty cycle for elevator motor
Share<float> yawC ("Current yaw from IMU");                 ///< A share containing current yaw of the glider
Share<float> pitchC ("Current pitch from IMU");             ///< A share containing current pitch of the glider

// Elevator Motor (Motor 0)
#define ELEVATOR_PIN_IN1   27       ///< GPIO 27 on ESP32: non-zero signal for (+) duty cycle
#define ELEVATOR_PIN_IN2   33       ///< GPIO 33 on ESP32: non-zero signal for (-) duty cycle
#define ELEVATOR_CHANNEL_A 2        ///< GPIO 2 on ESP32: unique channel
#define ELEVATOR_CHANNEL_B 3        ///< GPIO 3 on ESP32: unique channel

// Rudder Motor (Motor 1)
#define RUDDER_PIN_IN1   16         ///< GPIO 16 on ESP32: non-zero signal for (+) duty cycle
#define RUDDER_PIN_IN2   17         ///< GPIO 17 on ESP32: non-zero signal for (-) duty cycle
#define RUDDER_CHANNEL_A 0          ///< GPIO 0 on ESP32: unique channel
#define RUDDER_CHANNEL_B 1          ///< GPIO 1 on ESP32: unique channel

// Potentiometers
#define ELEVATOR_POT_PIN 34         ///< GPIO 34 on ESP32: reads voltage from elevator potentiometer
#define RUDDER_POT_PIN   39         ///< GPIO 39 on ESP32: reads voltage from rudder potentiometer

// Ultrasonic
#define TRIG 12                     ///< GPIO 12 on ESP32: ultrasonic trigger pin
#define ECHO 13                     ///< GPIO 1 on ESP32: ultrasonic echo pin

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

    // Distance
    float distance;

    // Height and timer threshold
    const uint8_t threshold = 20;               // cm

    // Create object
    Serial.println("Constructing the ultrasonic object");
    Ultrasonic ultra = Ultrasonic(ECHO, TRIG);

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
 *           to move the rudder and elevator control surfaces. The states within
 *           this task are set internally and by the webpage task.
 *  @param   p_params An unused pointer to (no) parameters passed to this task
 */
void task_controller (void* p_params)
{
    Serial << "Controller Task Begin" << endl;
  
    const uint8_t TASK_CONTROLLER_PERIOD = 50;  ///< Period of controller task (ms)

    // Controller objects
    PIDController yaw2rudder =      ///< Controller for rudder angle based on yaw
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD); 
    PIDController rudder2duty =     ///< Controller for duty cycle based on rudder angle
        PIDController(3,0,0,TASK_CONTROLLER_PERIOD);
    PIDController pitch2elev =      ///< Controller for elevator angle based on pitch
        PIDController(1,0,0,TASK_CONTROLLER_PERIOD);
    PIDController elev2duty =       ///< Controller for duty cycle based on elevator angle
        PIDController(3,0,0,TASK_CONTROLLER_PERIOD);

    // Create potentiometer object and zero the current reading
    Potentiometer rudderPot = Potentiometer(RUDDER_POT_PIN, 0);
    rudderPot.zero();

    // Create potentiometer object and zero the current reading
    Potentiometer elevPot = Potentiometer(ELEVATOR_POT_PIN, 0);
    elevPot.zero();

    // Initialize variables
    float yawD;                     ///< Desired yaw (deg)
    float pitchD;                   ///< Desired pitch (deg)  

    // Variables to keep track of previous potentiometer reading
    float prev_rudder = 0;          ///< Rudder position at previous time (deg)
    float prev_elevator = 0;        ///< Elevator position at previous time (deg)

    float rudderAngleD;             ///< Desired rudder angle (deg)
    float rudderAngleC;             ///< Current rudder angle (deg)
    float rudderAngleMin = -50;     ///< Minimum allowable rudder angle (deg)
    float rudderAngleMax = 50;      ///< Maximum allowable rudder angle (deg)

    float elevAngleD;               ///< Desired elevator angle (deg)
    float elevAngleC;               ///< Current elevator angle (deg)
    float elevAngleMin = -50;       ///< Minimum allowable elevator angle (deg)
    float elevAngleMax = 50;        ///< Maximum allowable elevator angle (deg)

    float rudderDutyD;              ///< Rudder motor duty cycle (-100% to 100% incl.)
    float elevDutyD;                ///< Elev motor duty cycle (-100% to 100% incl.)

    uint16_t delay_time = 0;        ///< Current amount of time (ms) in inactive delay
    tc_state.put(0);                // Initialize at state 0


    // Establish initial conditions for rudder and elevator
    rudderAngleC = rudderPot.get_angle();
    prev_rudder = rudderAngleC;

    elevAngleC = elevPot.get_angle();
    prev_elevator = elevAngleC;

    
    while (true) 
    {

        if (web_calibrate.get()) {        // If the webpage calls for calibration

            rudderPot.zero();             // Stop power to motors
            elevPot.zero();

            web_calibrate.put(0);         // Reset the calibrate flag

            Serial << "   Calibrated" << endl;

        }


        if (tc_state.get() == 0)          // STATE 0: DISABLED
        {        

            delay_time = 0;               // Reset delay counter
            rudder_duty.put(0);           // Stop power to motors
            elev_duty.put(0);

            // Passive state waiting for external callback to switch state
            Serial.println(" 0 ");

        }
        else if (tc_state.get() == 1)     // STATE 1: WAIT FOR LAUNCH
        {

            Serial.println(" 1 ");

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

                tc_state.put(2);       // Move to active state
                delay_time = 0;     // Reset counter
            }

        }
        else if (tc_state.get() == 2)     // STATE 2: CONTROLLER ACTIVE
        {

            // Time how long the plane is near the ground
            if (near_ground.get() == 1) 
            {
                delay_time += TASK_CONTROLLER_PERIOD;         
            }  
            else
            {
                delay_time = 0;
            }

            // If total delay time has reached 2000 ms...
            if (delay_time >= 2000) 
            {
                tc_state.put(0);         // Move to deactivated state
                delay_time = 0;          // Reset counter
            }

            // Check whether the glider is near ground
            if (near_ground.get()) 
            {
                pitchD = 10;   // If it is, set pitch
            }
            else 
            {
                pitchD = 0;    // If not, set different pitch
            }

            yawD = 0;          
        
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
            // Check if the difference in rudder angles are less than 30 degrees
            // to prevent undesired response to flickering measurements
            if (fabs(rudderAngleC - prev_rudder) < 30)
            {
                // Serial.println(fabs(rudderAngleC - prev_rudder));
                prev_rudder = rudderAngleC;
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
                    rudder_duty.put((int16_t) round(rudderDutyD));
                }
            }
            else
            {
                rudder_duty.put(0);
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
            if (fabs(elevAngleC - prev_elevator) < 30)
            {
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
            else
            {
                elev_duty.put(0);
            }

            Serial << "C: " << elevAngleC << "; D: " << elevAngleD << "; Duty: " << elev_duty.get() << endl;

            prev_elevator = elevAngleC;
            prev_rudder = rudderAngleC;

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
    const uint8_t period = 50;

    Serial << "Rudder Motor Task Begin" << endl;
    // Create object
    DRV8871 rudder = DRV8871(RUDDER_PIN_IN1, RUDDER_PIN_IN2, RUDDER_CHANNEL_A, RUDDER_CHANNEL_B);

    rudder.set_duty(0);

    while (true)
    {
        // Serial.println(rudder_duty.get());
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
    const uint8_t period = 50;

    Serial << "Elevator Motor Task Begin" << endl;
    // Create object
    DRV8871 elevator = DRV8871(
        ELEVATOR_PIN_IN1, ELEVATOR_PIN_IN2, ELEVATOR_CHANNEL_A, ELEVATOR_CHANNEL_B);

    elevator.set_duty(0);

    while (true)
    {
      elevator.set_duty(elev_duty.get());
      vTaskDelay(period);
    }
}

/** @brief   Task function to interface with IMU
 *  @details This task reads from the IMU to get pitch, yaw, and roll
 *           measurements. It then puts the data into shaes for the 
 *           controller to use. 
 *  @param   p_params A pointer to parameters passed to this task. This 
 *           pointer is ignored; it should be set to @c NULL in the 
 *           call to @c xTaskCreate() which starts this task
 */
void task_IMU(void* p_params) 
{
    // INIT
    LSM6DSOX imu;
    // declare float
    float pitch, yaw, roll;

    // READ VALUES
    while(true)
    {

        // SEND IT AND THE DATA BACK IN RADIANS
        imu.get_angle((float)time(0), pitch, yaw, roll);

        // Serial << "P: " << pitch*180/M_PI << ";  R: " << roll*180/M_PI << endl;

        // PUT ANGLES TO SHARES FOR CONTROLLER
        pitchC.put(pitch*180/M_PI);
        yawC.put(roll*180/M_PI);

        // PRINT IT
        // Serial << pitch * 180/M_PI << ", " << yaw * 180/M_PI << ", " << roll * 180/M_PI << endl;
        
        vTaskDelay(1);
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

    // start i2c
    Wire.begin();

    // Setup webpage
    setup_wifi();

    // Initialize web_calibrate to zero
    web_calibrate.put(1);

    // Task which runs the web server. It runs at a low priority
    xTaskCreate (task_webserver, "Web Server", 8192, NULL, 10, NULL);

    // Task for the potentiometer testing
    xTaskCreate (task_rudder_motor, "Rudder Motor", 2048, NULL, 20, NULL);

    // Task for the potentiometer testing
    xTaskCreate (task_elevator_motor, "Elevator Motor", 2048, NULL, 40, NULL);
    
    // Task for the ultrasonic sensor
    xTaskCreate (task_ultrasonic, "Ultrasonic Sensor", 2048, NULL, 50, NULL);

    // Task for the flight surface controls (rudder and elevator)
    xTaskCreate (task_controller, "Flight Controls", 2048,  NULL, 60, NULL);

    // Task for the IMU readings
    xTaskCreate (task_IMU, "IMU", 2048, NULL, 30, NULL);
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