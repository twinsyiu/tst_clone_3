// UltraSound module

#ifndef ULTRASOUND_H
#define ULTRASOUND_H

const int l_trigPin = 11; 
const int f_trigPin = 8; 
const int r_trigPin = 10; 

const int com_echoPin = 3;    // common echo pin of three ultrasound sensors

const int L_ULTA_ID = 0;
const int F_ULTA_ID = 1;
const int R_ULTA_ID = 2;

unsigned int ulta_trigPin[3] = { l_trigPin, f_trigPin, r_trigPin };

unsigned long echo_hi_ts_us = 0;
unsigned long echo_lo_ts_us = 0;


void ultrasound_init( void );
void ults_echo_chg_isr( void );
void ultrasound_trigger( int ulta_id  );


#endif
