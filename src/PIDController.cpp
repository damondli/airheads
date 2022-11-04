/** @file PIDController.cpp
 *  @author Shea Charkowsky and ME 507 Airheads team, based on format
 *  by Kane Stoboi and John Ridgely
 *  @date 2022-Nov-3
 *  @brief PID controller for plane motion
 *  @details Controls the attitude of a plane by applying PID control
 *  to the angle of the appropriate control surface.
 */

#include <Arduino.h>
#include "PIDController.h"
#include "taskshare.h"
#include "math.h"


/** @brief Initialize PIDController class
 */
PIDController::PIDController() 
{

}



/** @brief Set gains to user-inputted values
 */
void PIDController::setGains(float Kp, float Ki, float Kd) 
{
    PIDController::Kp = Kp;
    PIDController::Ki = Ki;
    PIDController::Kd = Kd;
}

/** @brief Calculate control surface angle outputted by the
 *  PID controller
 */
float PIDController::getCtrlSurfAngle() 
{
    // Get angle from IMU
    // Get angular velocity from IMU
    // Find error in angle and add that to error integral
    // Calculate product of gains and errors
    // Add them to find control surface angle
    // Return control surface angle
}

// We can determine the desired position from a separate function within 
// this class
// Current position will be retrieved from the potentiometer
int8_t PIDController::motorOutput(float currPos, float desPos)
{
    // Calculate the differenct between the desired and current position
    float difference = desPos - currPos;
    // Multiply by the proportional gain, implement Kd and Ki if needed
    int8_t duty_cycle = round(difference * k_motor);
    return duty_cycle;
}




