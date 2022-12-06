/** @file shares.h
 *  This file contains extern declarations of shares and queues which are used
 *  in more than one file of a demonstration project.
 * 
 *  @author JR Ridgely
 *  @author Damond Li edited file to be used for Airhead's ME507 Glider Project
 *  @date   2021-Oct-23 Original file
 *  @copyright (c) 2021 by JR Ridgely, released under the LGPL 3.0. 
 */

// Compile this file only once
#ifndef _SHARES_H_
#define _SHARES_H_

#include "taskqueue.h"
#include "taskshare.h"

extern Share<bool> near_ground;         ///< A share describing whether the glider is near the ground
extern Share<uint8_t> tc_state;         ///< A share describing the state of the controller FSM
extern Share<int16_t> rudder_duty;      ///< A share for the duty cycle for the rudder motor
extern Share<int16_t> elev_duty;        ///< A share for the duty cycle for the elevator motor
extern Share<float> yawC;               ///< A share for the current yaw
extern Share<float> pitchC;             ///< A share for the current pitch
extern Share<bool> web_calibrate;       ///< A share for a calibration variable

#endif // _SHARES_H_