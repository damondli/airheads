/** @file PIDController.cpp
 *  @author Shea Charkowsky and ME 507 Airheads team, based on format
 *  by Kane Stoboi and John Ridgely
 *  @date 2022-Nov-3
 *  @brief Generic PID controller class
 */

#include <Arduino.h>
#include "PIDController.h"
#include "taskshare.h"
#include "math.h"


/** @brief Initialize PIDController class
 */
PIDController::PIDController(float Kp, float Ki, float Kd, float dt) 
{
    PIDController::Kp = Kp;
    PIDController::Ki = Ki;
    PIDController::Kd = Kd;
    PIDController::dt = dt;

    PIDController::errIntegral = 0;      // Reset integral of error
    PIDController::errPrev = 0;          // Reset error at previous time
}


/** @brief Set gains to user-inputted values
 */
void PIDController::setGains(float Kp, float Ki, float Kd) 
{
    PIDController::Kp = Kp;
    PIDController::Ki = Ki;
    PIDController::Kd = Kd;
}


/** @brief Calculate PID control output at current time
 */
float PIDController::getControlOutput(float posCurrent, float posDesired) 
{
    float err = posDesired - posCurrent;
    float ierr = PIDController::errIntegral + err*PIDController::dt;
    float derr = (err - PIDController::errPrev) / PIDController::dt;

    return PIDController::Kp*err + PIDController::Ki*ierr + PIDController::Kd*derr;

    PIDController::errPrev = err;
}



