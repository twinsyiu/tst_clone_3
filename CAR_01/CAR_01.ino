#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
//#define SERIAL_DBG_ON   // enable serial monitor output debug message
#include "RGBLED_Hdlr.h"
#include "ClapDetect.h"
#include "UltraSound.h"


// the setup function runs once when you press reset or power the board
void setup() {

  RGB_LED_init();
  RGB_LED_set(COLOUR_WHITE);
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // Now set up Tasks to run independently.
  xTaskCreate(
    TaskUltraSoundRead
    ,  (const portCHAR *)"TaskUltraSoundRead"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );
  TaskUltraSound_Init();
  Serial.println("TaskUltraSoundRead: CREATED"); 
  
  xTaskCreate(
    TaskClapDetect
    ,  (const portCHAR *) "TaskClapDetect"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  TaskClapDetect_init();
  Serial.println("TaskClapDetect: CREATED"); 

  xTaskCreate(
    TaskCommander
    ,  (const portCHAR *) "TaskCommander"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  TaskCommander_init();
  Serial.println("TaskCommander: CREATED"); 

  xTaskCreate(
    TaskMotionCtrl
    ,  (const portCHAR *) "TaskMotionCtrl"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  
  TaskMotionCtrl_init();
  Serial.println("TaskMotionCtrl: CREATED"); 
  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.

}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/



