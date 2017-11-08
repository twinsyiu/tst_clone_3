#include "UltraSound.h"

#ifdef SERIAL_DBG_ON    
//#undef SERIAL_DBG_ON    
#endif

#define SOUND_SPEED_CM_PER_US (0.0343)
#define ULTRASOUND_SNSR_TIMEOUT_US (10000)  // ie object is 1.35m or more away from ultrasound sensor

void ultrasound_init( void )
{
  pinMode(f_trigPin, OUTPUT); 
  pinMode(com_echoPin, INPUT); 
  pinMode(r_trigPin, OUTPUT); 
  pinMode(l_trigPin, OUTPUT); 
}

long int start_us_ts;

void ultrasound_trig( unsigned int trigger_pin ) 
{
  // start_us_ts = micros();

  // put your main code here, to run repeatedly:
  digitalWrite(trigger_pin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigger_pin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigger_pin, LOW); 
}

float ulta_dist;

float ultrasound_read_cm( int ulta_id  ) 
{
  float duration_us, distance_cm; 
  long int stop_us_ts;

  echo_hi_ts_us = 0;
  echo_lo_ts_us = 0;

  //ultrasound_trig();
  // put your main code here, to run repeatedly:
  digitalWrite(ulta_trigPin[ulta_id], LOW); 
  delayMicroseconds(2); 
  digitalWrite(ulta_trigPin[ulta_id], HIGH); 
  delayMicroseconds(10); 
  digitalWrite(ulta_trigPin[ulta_id], LOW); 

  delay(20);

  ulta_dist = -1;

  // when the folloing conditions met, then it is a valid ultrasound measurement
  //   1. echo_hi_ts_us (micros timestamp) is not 0, ie there was a lo->hi change on the common-echo pin
  //   2. echo_lo_ts_us (micros timestamp) is not 0, ie there was a hi->lo change on the common-echo pin
  //   3. echo_lo_ts_us > echo_hi_ts_us, ie the edge changes was from lo->hi then hi->lo
  //   4. digitalRead(com_echoPin) is lo
  
  if ( echo_hi_ts_us && echo_lo_ts_us && (echo_lo_ts_us > echo_hi_ts_us ) && !digitalRead(com_echoPin) )
  {
    // this is a valid return from the ultrasound sensor
    //Serial.print("front dist: "); Serial.println( constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399) );
    // max 399cm ie 
    ulta_dist = constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399);
    return ulta_dist;
  } else if ( !echo_hi_ts_us )
  {
    Serial.println("ERROR dist: echo_hi_ts_us is 0"); 
  } else if ( !echo_lo_ts_us )
  {
    Serial.println("echo_lo_ts_us is 0, out of range, MAX 399cm"); 
    return 399;
  } else if ( echo_hi_ts_us >= echo_lo_ts_us )
  {
    Serial.println("ERROR dist: echo_hi_ts_us >= echo_lo_ts_us"); 
  } else if (digitalRead(com_echoPin))
  {
    Serial.println("ERROR dist: com_echoPin is HI"); 
  } else
  {
    Serial.println("ERROR dist: unknown error"); 
  }
  
  return -1.0;
 

}

