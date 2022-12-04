/** @file PIDController.h
 *  @brief Generic PID controller class (header file)
 *  @author Shea Charkowsky and ME 507 Airheads team, based on format
 *  by Kane Stoboi and John Ridgely and method by Siyuan Xing
 *  @date 2022-Nov-03
 */

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <Arduino.h>
#include "taskshare.h"

/** @brief  Class for a proportional, intergral, and derivative (PID) controller
 */
class PIDController
{
protected:

    float Kp;               ///< Proportional gain
    float Ki;               ///< Integral gain
    float Kd;               ///< Derivative gain

    float dt;               ///< Sampling interval

    float errIntegral;      ///< Integral of error
    float errPrev;          ///< Error at previous time

public:
    PIDController(float Kp, float Ki, float Kd, float dt);          ///< Constructor for PID Controller class

    void setGains(float Kp, float Ki, float Kd);                    ///< Method to set/update the controller gains
    float getCtrlOutput(float posCurrent, float posDesired);        ///< Method to run the controller, returns controller output
};

#endif // _CONTROLLER_H_