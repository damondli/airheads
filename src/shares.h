/** @file shares.h
 *  This file contains extern declarations of shares and queues which are used
 *  in more than one file of a demonstration project.
 * 
 *  @author JR Ridgely
 *  @date   2021-Oct-23 Original file
 *  @copyright (c) 2021 by JR Ridgely, released under the LGPL 3.0. 
 */

#ifndef _SHARES_H_
#define _SHARES_H_

#include "taskqueue.h"
#include "taskshare.h"

// A share describing whether the glider is near the ground
extern Share<bool> near_ground;

// A share for the duty cycle for the rudder motor
extern Share<int16_t> rudder_duty ("Rudder motor duty cycle");

// A share for the duty cycle for the elevator motor
extern Share<int16_t> elev_duty ("Elevator motor duty cycle");

// A queue which triggers a task to print the count at certain times
extern Queue<uint16_t> data_queue;

#endif // _SHARES_H_