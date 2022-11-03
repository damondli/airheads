/** @file PIDController.h
 *  @author Shea Charkowsky and ME 507 Airheads team, based on format
 *  by Kane Stoboi and John Ridgely
 *  @date 2022-Nov-3
 */

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <Arduino.h>
#include "taskshare.h"


class PIDController
{
protected:

    float Kp;           // Proportional gain
    float Ki;           // Integral gain
    float Kd;           // Derivative gain

    float errIntegral;  // Integral of error in angle

public:
    PIDController(/* Arguments...? */);

    void setGains(float Kp, float Ki, float Kd);
    float getCtrlSurfAngle(void);

};

#endif // _CONTROLLER_