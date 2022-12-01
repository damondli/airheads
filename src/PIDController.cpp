/** @file PIDController.cpp
 *  @author Shea Charkowsky and ME 507 Airheads team, based on format
 *  by Kane Stoboi and John Ridgely
 *  @date 2022-Nov-3
 *  @brief Generic PID controller class
 */

#include <Arduino.h>
#include "PIDController.h"
#include "math.h"


/** @brief Initialize PIDController class
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
 */
void PIDController::setGains(float Kp_in, float Ki_in, float Kd_in) 
{
    Kp = Kp_in;
    Ki = Ki_in;
    Kd = Kd_in;
}


/** @brief Calculate PID control output at current time
 */
float PIDController::getCtrlOutput(float posCurrent, float posDesired) 
{
    float err = posDesired - posCurrent;
    errIntegral += err*dt;
    float derr = (err - errPrev) / dt;

    return ( Kp*err 
           + Ki*errIntegral 
           + Kd*derr );

    errPrev = err;
}



