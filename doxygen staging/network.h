/** @file network.h
 *  @brief This header file contains the functions that main.cpp will need
 *         in order to create a web page. This 
 * 
 *  @author Damond Li
 *  @date 2022-Nov-28 Original file
 */

#ifndef _NETWORK_
#define _NETWORK_

#include "shares.h"
#include "taskshare.h"

/** @brief  Function used to setup the wifi on the ESP32
 */
void setup_wifi(void);

/** @brief  The webpage task that runs the webpage on the ESP32
 */
void task_webserver (void* p_params);

#endif // _NETWORK_