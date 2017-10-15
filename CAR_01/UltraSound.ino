#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include "UltraSound.h"

//#define SERIAL_DBG_ON    

#ifdef SERIAL_DBG_ON    
//#undef SERIAL_DBG_ON    
#endif

#define SOUND_SPEED_CM_PER_US (0.0343)
#define ULTRASOUND_SNSR_TIMEOUT_US (10000)  // ie object is 1.35m or more away from ultrasound sensor

void TaskUltraSound_Init( void )
{
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

    // Now set up Tasks to run independently.
  xTaskCreate(
    TaskUltraSoundRead
    ,  (const portCHAR *)"TaskUltraSoundRead"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );
}


void TaskUltraSoundRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  float duration_us, distance_cm; 

  for (;;) // A Task shall never return or exit.
  {
    // put your main code here, to run repeatedly:
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2); 
    digitalWrite(trigPin, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW); 
    
    duration_us = pulseIn(echoPin, HIGH, ULTRASOUND_SNSR_TIMEOUT_US); // Returns the length of the pulse in microseconds

    if (duration_us)
    {
      obstacle_distance_cm = distance_cm = (duration_us * SOUND_SPEED_CM_PER_US)/2; 
    }
    else
    {
      obstacle_distance_cm = distance_cm = OUT_OF_RANGE_DIST_CM;
    }
    
    
#ifdef SERIAL_DBG_ON    
    Serial.print("duration(us): "); 
    Serial.print(duration_us); 
    Serial.print("   Distance(cm): "); 
    Serial.println(distance_cm); 
#endif
    //vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
    vTaskDelay(50 / portTICK_PERIOD_MS);  // nof tick delay (200ms)
  }
}

