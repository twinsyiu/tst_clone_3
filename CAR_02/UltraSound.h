#ifndef ULTRASOUND_H
#define ULTRASOUND_H

#define OUT_OF_RANGE_DIST_CM 200
// UltraSound module
const int trigPin = 8; 
//const int x1echoPin = 9;
const int r_trigPin = 10; 
const int com_echoPin = 3;
const int l_trigPin = 11; 
//const int x2echoPin = 3;


// this is the only output from this ultrasound sensor
float obstacle_distance_cm = OUT_OF_RANGE_DIST_CM;

/*
void TaskUltraSoundRead( void *pvParameters );
void TaskUltraSound_Init( void );
*/

void ultrasound_init( void );
void ultrasound_trig( void ) ;
float ultrasound_read_cm( void ) ;


#endif
