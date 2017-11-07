#ifndef ULTRASOUND_H
#define ULTRASOUND_H

#define OUT_OF_RANGE_DIST_CM 200
// UltraSound module
const int l_trigPin = 11; 
const int f_trigPin = 8; 
const int r_trigPin = 10; 

const int com_echoPin = 3;

const int L_ULTA_ID = 0;
const int F_ULTA_ID = 1;
const int R_ULTA_ID = 2;

unsigned int ulta_trigPin[3] = { l_trigPin, f_trigPin, r_trigPin };


// this is the only output from this ultrasound sensor
float obstacle_distance_cm = OUT_OF_RANGE_DIST_CM;

void ultrasound_init( void );
void ultrasound_trig( void ) ;
float ultrasound_read_cm( int ulta_id ) ;


#endif
