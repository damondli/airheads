/** @file PIDController.cpp
 *  @author Shea Charkowsky and ME 507 Airheads team, based on format
 *  by Kane Stoboi and John Ridgely
 *  @date 2022-Nov-03
 *  @brief Generic PID controller class
 */

#include <Arduino.h>
#include "PIDController.h"
#include "math.h"


/** @brief Initialize PIDController class
 *  @param Kp_in Proprotional gain
 *  @param Ki_in Integral gain
 *  @param Kd_in Derivative gain
 *  @param dt_in Interval at which the controller is run
 */
PIDController::PIDController(float Kp_in, float Ki_in, float Kd_in, float dt_in) 
{
    Kp = Kp_in;
    Ki = Ki_in;
    Kd = Kd_in;
    dt = dt_in;

    errIntegral = 0;      // Reset integral of error
    errPrev = 0;          // Reset error at previous time
}


/** @brief Set gains to user-inputted values
 *  @param Kp_in Proprotional gain
 *  @param Ki_in Integral gain
 *  @param Kd_in Derivative gain
 */
void PIDController::setGains(float Kp_in, float Ki_in, float Kd_in) 
{
    Kp = Kp_in;
    Ki = Ki_in;
    Kd = Kd_in;
}


/** @brief Calculate PID control output at current time
 *  @param posCurrent The current value or position that is being measured
 *  @param posDesired The desired value or position that the actuator should be at
 *  @returns The controller output
 */
float PIDController::getCtrlOutput(float posCurrent, float posDesired) 
{
    // Calculate error
    float err = posDesired - posCurrent;
    // Update the integral error
    errIntegral += err*dt;
    // Calculate the derivative error
    float derr = (err - errPrev) / dt;
    // Update previous error
    errPrev = err;

    // Return summation of errors multiplied by their respective gains
    return ( Kp*err 
           + Ki*errIntegral 
           + Kd*derr );
}



