#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
//#define SERIAL_DBG_ON   // enable serial monitor output debug message
#include "RGBLED_Hdlr.h"
#include "ClapDetect.h"

// UltraSound module
const int trigPin = 6; 
const int echoPin = 7;
float duration, distance; 


// define two Tasks for DigitalRead & AnalogRead
void TaskUltraSoundRead( void *pvParameters );
//void TaskClapDetect( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {

  RGB_LED_init();
  RGB_LED_set(COLOUR_WHITE);
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  // using RISING edge on sound level sensor
//  attachInterrupt(digitalPinToInterrupt(interruptPin), log_chg, RISING);

  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskUltraSoundRead
    ,  (const portCHAR *)"TaskUltraSoundRead"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );
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
  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskUltraSoundRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  for (;;) // A Task shall never return or exit.
  {
    // put your main code here, to run repeatedly:
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2); 
    digitalWrite(trigPin, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW); 
    
    duration = pulseIn(echoPin, HIGH); 
    
    distance = (duration*.0343)/2; 
#ifdef SERIAL_DBG_ON    
    Serial.print("Distance: "); 
    Serial.print(distance); 
    Serial.println(" cm"); 
    //delay(1000); 
#endif
    //vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
    vTaskDelay(200 / portTICK_PERIOD_MS);  // nof tick delay (200ms)
  }
}


