#ifndef ULTRASOUND_H
#define ULTRASOUND_H

#define OUT_OF_RANGE_DIST_CM 200
// UltraSound module
const int trigPin = 9; 
const int echoPin = 10;

// this is the only output from this ultrasound sensor
float obstacle_distance_cm = OUT_OF_RANGE_DIST_CM;

void TaskUltraSoundRead( void *pvParameters );
void TaskUltraSound_Init( void );

#endif
