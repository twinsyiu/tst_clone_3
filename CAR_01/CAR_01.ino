#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include "RGBLED_Hdlr.h"
#include "ClapDetect.h"
#include "UltraSound.h"


// the setup function runs once when you press reset or power the board
void setup() {

  RGB_LED_init();
  RGB_LED_set(COLOUR_WHITE);
  
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  TaskUltraSound_Init();
  // Serial.println("TaskUltraSoundRead: CREATED"); 
  
  TaskClapDetect_init();
  // Serial.println("--TaskClapDetect: CREATED"); 

  xTaskCreate(
    TaskCommander
    ,  (const portCHAR *) "TaskCommander"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  TaskCommander_init();
  // Serial.println("TaskCommander: CREATED"); 

  xTaskCreate(
    TaskMotionCtrl
    ,  (const portCHAR *) "TaskMotionCtrl"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  
  TaskMotionCtrl_init();
  // Serial.println("TaskMotionCtrl: CREATED"); 
  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.

}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/



