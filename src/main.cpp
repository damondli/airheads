
/** @file main.cpp
 */

#include <Arduino.h>
#include "DRV8871.h"
#include "PrintStream.h"

// Motor 1
#define M1_PIN_A 16
#define M1_PIN_B 13
#define CHANNEL_M1A 0
#define CHANNEL_M1B 1

// Motor 2
#define M2_PIN_A 32
#define M2_PIN_B 33
#define CHANNEL_M2A 2
#define CHANNEL_M2B 3


/** @brief   
 *  @details 
 *  @param   p_params A pointer to parameters passed to this task. This 
 *           pointer is ignored; it should be set to @c NULL in the 
 *           call to @c xTaskCreate() which starts this task
 */
void task_motor_A (void* p_params)
{
    Serial << "Motor driver A is set. " << endl;

    //DRV8871 motor_A = DRV8871(M1_PIN_A, M1_PIN_B, CHANNEL_M1A, CHANNEL_M1B);

    uint8_t duty = 50;

    while (true)
    {
      //motor_A.set_duty(duty);
      vTaskDelay(500);
    }
}



/** @brief   
 *  @details 
 *  @param   p_params An unused pointer to (no) parameters passed to this task
 */
void task_motor_B (void* p_params)
{
    Serial << "Motor driver B is set. " << endl;

    DRV8871 motor_B = DRV8871(M2_PIN_A, M2_PIN_B, CHANNEL_M2A, CHANNEL_M2B);

    while (true)
    {
      motor_B.set_duty(75);
      Serial.print(motor_B.duty);
      vTaskDelay(500);
    }
}


/** @brief   The Arduino setup function.
 *  @details This function is used to set up the microcontroller by starting
 *           the serial port and creating the tasks.
 */
void setup (void) 
{
    // The serial port must begin before it may be used
    Serial.begin (115200);

    // Wait for serial to finish setting up before proceeding
    while (!Serial) 
    {
    }

    Serial << "Serial is ready. " << endl;

    // Create the task which outputs the high frequency square wave.
    // xTaskCreate (task_motor_A, "Motor A", 2048, NULL, 4, NULL);
    
    // Create the task which outputs the low frequency square wave.
    // xTaskCreate (task_motor_B, "Motor B", 2048, NULL, 3, NULL);

    DRV8871 motor_A = DRV8871(M1_PIN_A, M1_PIN_B, CHANNEL_M1A, CHANNEL_M1B);
    motor_A.set_duty(50);

}


/** @brief   The Arduino loop function.
 *  @details This function is called periodically by the Arduino system. It
 *           runs as a low priority task. On some microcontrollers it will
 *           crash when FreeRTOS is running, so we usually don't use this
 *           function for anything. 
 */

void loop (void)
{
}