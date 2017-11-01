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

float ultrasound_read_cm( void ) 
{
  float duration_us, distance_cm; 
  long int stop_us_ts;
  
  if ( digitalRead(com_echoPin) )
  {
    stop_us_ts = micros();
    distance_cm = ((stop_us_ts - start_us_ts) * SOUND_SPEED_CM_PER_US)/2;
    return distance_cm;
  }
  else
  {
    return 0;
  }



}

