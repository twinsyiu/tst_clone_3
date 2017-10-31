#include "motor_ctlr.h"
#include "encoder.h"
#include "UltraSound.h"

float vcc_volt;
const byte ults_interrupt_pin = 3;
int ults_echo_chg_cnt = 0;
unsigned long echo_hi_ts_us = 0;
unsigned long echo_lo_ts_us = 0;

void setup() 
{
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  Serial.print("A0 : ");
  Serial.println(analogRead(A0));
  
  Serial.print("A0 : **** ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);
/*
  if (vcc_volt < 9.0)
  {
    Serial.print("LOW POWER SUPPLY VOLTAGE !!! PLEASE RECHARGE BATTERY NOW ");
    while (1) {};
  }
*/
  ultrasound_init( );
  attachInterrupt(digitalPinToInterrupt(ults_interrupt_pin), ults_echo_chg, CHANGE);
  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  interrupts(); // enable all interrupts

}

void ults_echo_chg() 
{
  ults_echo_chg_cnt++;
  if ( digitalRead(com_echoPin) )
  {
    echo_hi_ts_us = micros();
  } 
  else
  {
    echo_lo_ts_us = micros();
  }
}


unsigned int ults_cntr;
unsigned int ults_trig = 0;
unsigned int ults_echo_state = 0;

unsigned int r_ults_cntr;
unsigned int r_ults_trig = 0;
unsigned int r_ults_echo_state = 0;

unsigned int l_ults_cntr;
unsigned int l_ults_trig = 0;
unsigned int l_ults_echo_state = 0;


ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
}

float f_dist, l_dist, r_dist;

void loop()
{
  int ms_ts;
  long duration;
  String stringOne;

  stringOne = "front dist: ";
  echo_hi_ts_us = 0;
  echo_lo_ts_us = 0;
  
  //ultrasound_trig();
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 

  // Serial.print("front dist: "); Serial.println( constrain(duration * 3.43 / 2 , 0 , 399) );
  delay(100);
  stringOne += ults_echo_chg_cnt;

  f_dist = -1;

  // when the folloing conditions met, then it is a valid ultrasound measurement
  //   1. echo_hi_ts_us (micros timestamp) is not 0, ie there was a lo->hi change on the common-echo pin
  //   2. echo_lo_ts_us (micros timestamp) is not 0, ie there was a hi->lo change on the common-echo pin
  //   3. echo_lo_ts_us > echo_hi_ts_us, ie the edge changes was from lo->hi then hi->lo
  //   4. digitalRead(com_echoPin) is lo
  
  if ( echo_hi_ts_us && echo_lo_ts_us && (echo_lo_ts_us > echo_hi_ts_us ) && !digitalRead(com_echoPin) )
  {
    // this is a valid return from the ultrasound sensor
    //Serial.print("front dist: "); Serial.println( constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399) );
    f_dist = constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399);
  } else if ( !echo_hi_ts_us )
  {
    Serial.println("front dist: echo_hi_ts_us is 0"); 
  } else if ( !echo_lo_ts_us )
  {
    Serial.println("front dist: echo_lo_ts_us is 0"); 
  } else if ( echo_hi_ts_us >= echo_lo_ts_us )
  {
    Serial.println("front dist: echo_hi_ts_us >= echo_lo_ts_us"); 
  } else if (digitalRead(com_echoPin))
  {
    Serial.println("front dist: com_echoPin is HI"); 
  } else
  {
    Serial.println("front dist: unknown error"); 
  }
 

  digitalWrite(l_trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(l_trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(l_trigPin, LOW); 

  // Serial.print("left dist: "); Serial.println( constrain(duration * 3.43 / 2 , 0 , 399) );
  delay(100);
  stringOne = "left dist: ";
  stringOne += ults_echo_chg_cnt;

  l_dist = -1;
  if ( echo_hi_ts_us && echo_lo_ts_us && (echo_lo_ts_us > echo_hi_ts_us ) && !digitalRead(com_echoPin) )
  {
    // Serial.print("left dist: "); Serial.println( constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399) );
    l_dist = constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399);
  } else if ( !echo_hi_ts_us )
  {
    Serial.println("left dist: echo_hi_ts_us is 0"); 
  } else if ( !echo_lo_ts_us )
  {
    Serial.println("left dist: echo_lo_ts_us is 0"); 
  } else if ( echo_hi_ts_us >= echo_lo_ts_us )
  {
    Serial.println("left dist: echo_hi_ts_us >= echo_lo_ts_us"); 
  } else if (digitalRead(com_echoPin))
  {
    Serial.println("left dist: com_echoPin is HI"); 
  } else
  {
    Serial.println("left dist: unknown error"); 
  }
 

  digitalWrite(r_trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(r_trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(r_trigPin, LOW); 

  delay(100);
  stringOne = "right dist: ";
  stringOne += ults_echo_chg_cnt;
  r_dist = -1;
  if ( echo_hi_ts_us && echo_lo_ts_us && (echo_lo_ts_us > echo_hi_ts_us ) && !digitalRead(com_echoPin) )
  {
    //Serial.print("right dist: "); Serial.println( constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399) );
    r_dist = constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399);
  } else if ( !echo_hi_ts_us )
  {
    Serial.println("right dist: echo_hi_ts_us is 0"); 
  } else if ( !echo_lo_ts_us )
  {
    Serial.println("right dist: echo_lo_ts_us is 0"); 
  } else if ( echo_hi_ts_us >= echo_lo_ts_us )
  {
    Serial.println("right dist: echo_hi_ts_us >= echo_lo_ts_us"); 
  } else if (digitalRead(com_echoPin))
  {
    Serial.println("right dist: com_echoPin is HI"); 
  } else
  {
    Serial.println("right dist: unknown error"); 
  }
 
  stringOne = "left/front/right dist: ";
  stringOne += l_dist; stringOne += " / ";
  stringOne += f_dist; stringOne += " / ";
  stringOne += r_dist;
  Serial.println(stringOne);
  
  
  delay(500);
}


//
// previous loop() to test the encoder feedback which adjust the left wheel PWM to ensure straight line movement.
//
void loop_111() 
{
  unsigned int motor_PWM;
  unsigned int l_motor_PWM;
  encoder_Struct encoder;

  int loop_cnt;
  int l_faster, prev_l_faster;

  int i;

  //
  // do motor reverse compensation using the encoder as feedback
  //
  motor_PWM = 15;               // initial test PWM = 15%
  l_motor_PWM = motor_PWM;      // left_motor_PWM = 15%

  // 7 steps to increase the motor PWM
  for ( loop_cnt = 7 ; loop_cnt > 0 ; loop_cnt-- )
  {
    reset_encoder_count();
    
    Serial.print("motor_reverse (%): "); Serial.print(motor_PWM); Serial.print("----- with auto compensation"); 
    motor_reverse(motor_PWM);    // 

    for ( i = 40 ; i > 0 ; i-- )
    {
      delay(200);     // the delay is a test feature/stub to kill time, in real application, other code (FreeRTOS tasks) will be running
      
      // l_faster : an integer flag to indicate if left wheel is faster 
      //   l_faster > 0 : left wheel is faster
      //   l_faster < 0 : left wheel is slower
      
      l_faster = get_encoder( &encoder );
      if ( l_faster > 0 )
      { //   l_faster > 0 : left wheel is faster
        // slow down the left wheel
        if ( prev_l_faster > 0 )
        {
          l_motor_PWM -= 2;     // double the decreament if prev sample indicating the left was faster already
        }
        else
        {
          l_motor_PWM -= 1;
        }
        motor_drive_sp( MOTOR_L, DIR_REV, l_motor_PWM );
        Serial.print("(-) ");
      } 
      else if ( l_faster < 0 )
      { //   l_faster < 0 : left wheel is slower
        // speed up the left wheel
        if ( prev_l_faster < 0 )
        {
          l_motor_PWM += 2;     // double the increament if prev sample indicating the left was slower already
        }
        else
        {
          l_motor_PWM += 1;
        }
        motor_drive_sp( MOTOR_L, DIR_REV, l_motor_PWM );
        Serial.print("(+) ");
      } // else equal, do nothing
      else
      {
        Serial.print("( ) ");
      } // else equal, do nothing
      prev_l_faster = l_faster;
      Serial.print(l_motor_PWM);
      Serial.print("    left enc cnt : "); Serial.print(encoder.l_encoder_count);
      Serial.print("    righ enc cnt : "); Serial.println(encoder.r_encoder_count);
    }
    motor_PWM += 10;
    l_motor_PWM = motor_PWM;
  }

  motor_stop();
  delay(4000);


  
  

  //------------------------------------------------------
  Serial.println("END OF TEST");

  while(1)
  {
  
  
  }
}

// Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) 
{

  encoder_update();

}


