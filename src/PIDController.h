/** @file PIDController.h
 *  @brief Generic PID controller class (header file)
 *  @author Shea Charkowsky and ME 507 Airheads team, based on format
 *  by Kane Stoboi and John Ridgely and method by Siyuan Xing
 */

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <Arduino.h>
#include "taskshare.h"


class PIDController
{
protected:

    float Kp;               // Proportional gain
    float Ki;               // Integral gain
    float Kd;               // Derivative gain

    float dt;               // Sampling interval

    float errIntegral;      // Integral of error
    float errPrev;          // Error at previous time

public:
    PIDController(float Kp, float Ki, float Kd, float dt);

    void setGains(float Kp, float Ki, float Kd);
    float getControlOutput(float posCurrent, float posDesired);
};

#endif // _CONTROLLER_H_