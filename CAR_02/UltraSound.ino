#include "UltraSound.h"

#ifdef SERIAL_DBG_ON
//#undef SERIAL_DBG_ON
#endif

void ultrasound_init( void )
{
  pinMode(f_trigPin, OUTPUT);   // front sensor trigger pin
  pinMode(r_trigPin, OUTPUT);   // right sensor trigger pin
  pinMode(l_trigPin, OUTPUT);   // left sensor trigger pin

  pinMode(com_echoPin, INPUT);  // common echo pin of three ultrasound sensors

  // configure the ultrasound sensors common echo line as intrrupt source when level change
  attachInterrupt(digitalPinToInterrupt(com_echoPin), ults_echo_chg_isr, CHANGE);

  // initialise the hi and lo timestamp variables
  echo_hi_ts_us = 0;
  echo_lo_ts_us = 0;

}

/*
 * ults_echo_chg_isr() is the ISR of ultrasound echo line level change
 */
void ults_echo_chg_isr( void )
{
  if ( digitalRead(com_echoPin) )
  {
    // when the echo pin is HI, record the hi-timestamp
    echo_hi_ts_us = micros();
  }
  else
  {
    // when the echo pin is LO, record the lo-timestamp
    echo_lo_ts_us = micros();
  }
}

/*
 * ultrasound_trigger() will fire a short pulse on the selected ultrasound sensor
 */
void ultrasound_trigger( int ulta_id  )
{
  echo_hi_ts_us = 0;
  echo_lo_ts_us = 0;

  //ultrasound_trig();
  // put your main code here, to run repeatedly:
  digitalWrite(ulta_trigPin[ulta_id], LOW);
  delayMicroseconds(2);
  digitalWrite(ulta_trigPin[ulta_id], HIGH);
  delayMicroseconds(10);
  digitalWrite(ulta_trigPin[ulta_id], LOW);
}

