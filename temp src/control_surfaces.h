/** @file control_surfaces.h
 *  This Arduino Library operates two DC motors in microservo bodies
 *  which move the rudder and elevator of a model airplane.
 *  @author ME 507 Airheads
 *  @date 2022-Nov-09
 */

#ifndef CONTROL_SURFACES
#define CONTROL_SURFACES

#include <Arduino.h>
#include <Wire.h>

void task_elevator (void* p_params);

void task_rudder (void* p_params);

#endif