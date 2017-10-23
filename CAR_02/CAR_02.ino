#include "motor_ctlr.h"
#include "encoder.h"
#include "UltraSound.h"

//#define ledPin 13

float vcc_volt;

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

  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

//  pinMode(ledPin, OUTPUT);
  
  // initialize Timer1
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
//  OCR1A = 31250; // compare match register 16MHz/256/2Hz
  OCR1A = 1600; // compare match register 16MHz/1/10kHz
  TCCR1B |= (1 << WGM12); // CTC mode
//  TCCR1B |= (1 << CS12); // 256 prescaler
  TCCR1B |= (1 << CS10); // 1 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts

//  ultrasound_init();
}

unsigned int high_cntr;

unsigned int ults_cntr;
unsigned int ults_trig = 0;
unsigned int ults_echo_state = 0;

unsigned int l_ults_cntr;
unsigned int l_ults_trig = 0;
unsigned int l_ults_echo_state = 0;


ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
//  high_cntr++;
  
  if ( ults_trig )
  {
    if ( !ults_echo_state && digitalRead(echoPin) )
    {
      ults_echo_state = 1;  // and start counting
    } else
    if ( ults_echo_state && digitalRead(echoPin) )
    {
      ults_cntr++;
    } else
    if ( ults_echo_state && !digitalRead(echoPin) )
    {
      ults_cntr++;
      ults_trig = 0;
    }
  }

  if ( l_ults_trig )
  {
    if ( !l_ults_echo_state && digitalRead(l_echoPin) )
    {
      l_ults_echo_state = 1;  // and start counting
    } else
    if ( l_ults_echo_state && digitalRead(l_echoPin) )
    {
      l_ults_cntr++;
    } else
    if ( l_ults_echo_state && !digitalRead(l_echoPin) )
    {
      l_ults_cntr++;
      l_ults_trig = 0;
    }
  }

}


void loop()
{
  int ms_ts;
  //Serial.print("high_cntr: ");Serial.println(high_cntr);
  
  //ultrasound_trig();
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  

  ults_cntr = 0;
  ults_trig = 1;
  ults_echo_state = 0;

  ms_ts = millis();

  while (ults_trig && (ults_cntr < 2000))
  {
    Serial.print("- ");
    if ( (millis() - ms_ts) > 10 )
    {
      //Serial.print("ults_cntr: ");Serial.println(ults_cntr);
      Serial.print("+ ");
      ms_ts = millis();
    }
  }   // do nothing when the trig is high AND not timeout 200ms
  Serial.print("%% ");
  
  if (ults_trig)
  {
    // timeout
    Serial.println("FRONT : TOO FAR or TIMEOUT ");
  }
  else
  {
//    Serial.print("front dist: "); Serial.println( ults_cntr * 3.43 / 2 );
  }

//  Serial.println("----------------");
  delay(10);

  digitalWrite(l_trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(l_trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(l_trigPin, LOW); 
  
  l_ults_cntr = 0;
  l_ults_trig = 1;
  l_ults_echo_state = 0;

  ms_ts = millis();

  while (l_ults_trig && (l_ults_cntr < 2000))
  {
    Serial.print("& ");
    //delay(2);
    if ( (millis() - ms_ts) > 10 )
    {
      Serial.print("# ");
      //Serial.print("l_ults_cntr: ");Serial.println(l_ults_cntr);
      ms_ts = millis();
    }
  }   // do nothing when the trig is high AND not timeout 200ms
  Serial.println("@ ");
  
  if (l_ults_trig)
  {
    // timeout
    Serial.println("LEFT : TOO FAR or TIMEOUT ");
  }
  else
  {
//    Serial.print("left dist: "); Serial.println( l_ults_cntr * 3.43 / 2 );
  }

    Serial.print("front dist: "); Serial.print( ults_cntr * 3.43 / 2 );
    Serial.print("   left dist: "); Serial.println( l_ults_cntr * 3.43 / 2 );

//  Serial.println("================");
  delay(500);
}




void loop_101()
{

  high_cntr = 0;
  delayMicroseconds(10000);   // delayMicroseconds() is not working in this scheme
  Serial.print("10ms: "); Serial.println(high_cntr);
  delay(100);

  high_cntr = 0;
  delayMicroseconds(20000);
  Serial.print("20ms: "); Serial.println(high_cntr);
  delay(100);

  high_cntr = 0;
  delayMicroseconds(30000);
  Serial.print("30ms: "); Serial.println(high_cntr);
  delay(100);

  high_cntr = 0;
  delayMicroseconds(40000);
  Serial.print("40ms: "); Serial.println(high_cntr);
  delay(100);

  
  
  high_cntr = 0;
  delay(10);
  Serial.print("10ms: "); Serial.println(high_cntr);

  high_cntr = 0;
  delay(20);
  Serial.print("20ms: "); Serial.println(high_cntr);

  high_cntr = 0;
  delay(30);
  Serial.print("30ms: "); Serial.println(high_cntr);

  high_cntr = 0;
  delay(40);
  Serial.print("40ms: "); Serial.println(high_cntr);

  high_cntr = 0;
  delay(70);
  Serial.print("70ms: "); Serial.println(high_cntr);

  high_cntr = 0;
  delay(100);
  Serial.print("100ms: "); Serial.println(high_cntr);

  while(1) {};
  
}

void loop_111() 
{
  // put your main code here, to run repeatedly:
  unsigned int motor_PWM;
  unsigned int l_motor_PWM;
  encoder_Struct encoder;

  int loop_cnt;
  int l_faster, prev_l_faster;

  int i;
/*  
  motor_PWM = 15;
  
  for ( loop_cnt = 4 ; loop_cnt > 0 ; loop_cnt-- )
  {
    reset_encoder_count();
    Serial.print("motor_forward (%): "); Serial.print(motor_PWM); Serial.print("-----"); 
    motor_forward(motor_PWM);    // 

    for ( i = 20 ; i > 0 ; i-- )
    {
      delay(200);
      l_faster = get_encoder( &encoder );
      if ( l_faster > 0 )
      {
        // slow down the left wheel
      } else if ( l_faster < 0 )
      {
        // speed up the left wheel
      } // else equal, do nothing
      Serial.print("left enc cnt : "); Serial.print(encoder.l_encoder_count);
      Serial.print("    righ enc cnt : "); Serial.println(encoder.r_encoder_count);
    }
    motor_PWM += 10;
  }

  motor_stop();
  delay(4000);
*/


  motor_PWM = 15;
  l_motor_PWM = motor_PWM;
  
  for ( loop_cnt = 7 ; loop_cnt > 0 ; loop_cnt-- )
  {
    reset_encoder_count();
    Serial.print("motor_reverse (%): "); Serial.print(motor_PWM); Serial.print("----- with auto compensation"); 
    motor_reverse(motor_PWM);    // 

    for ( i = 40 ; i > 0 ; i-- )
    {
      delay(200);
      l_faster = get_encoder( &encoder );
      if ( l_faster > 0 )
      {
        // slow down the left wheel
        if ( prev_l_faster > 0 )
        {
          l_motor_PWM -= 2;
        }
        else
        {
          l_motor_PWM -= 1;
        }
        motor_drive_sp( MOTOR_L, DIR_REV, l_motor_PWM );
        Serial.print("(-) ");
      } 
      else if ( l_faster < 0 )
      {
        // speed up the left wheel
        if ( prev_l_faster < 0 )
        {
          l_motor_PWM += 2;
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


  while(1) {};

  
  
  
  motor_PWM = 15;
  
  for ( loop_cnt = 7 ; loop_cnt > 0 ; loop_cnt-- )
  {
    reset_encoder_count();
    Serial.print("motor_reverse (%): "); Serial.print(motor_PWM); Serial.print("-----"); 
    motor_reverse(motor_PWM);    // 

    for ( i = 20 ; i > 0 ; i-- )
    {
      delay(200);
      l_faster = get_encoder( &encoder );
      if ( l_faster > 0 )
      {
        // slow down the left wheel
      } else if ( l_faster < 0 )
      {
        // speed up the left wheel
      } // else equal, do nothing
      Serial.print("left enc cnt : "); Serial.print(encoder.l_encoder_count);
      Serial.print("    righ enc cnt : "); Serial.println(encoder.r_encoder_count);
    }
    motor_PWM += 10;
  }

  motor_stop();
  delay(4000);


  
  
  while(1) {};









  Serial.println("motor_forward(15%)");
  motor_PWM = 15;
  motor_forward(motor_PWM);    // 
  start_encoder_count(5000);

  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  Serial.println("motor_stop(0%)");
  motor_stop();    // 0%
  delay(2000);
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);


  Serial.println("motor_forward(20%)");
  motor_PWM = 20;
  motor_forward(motor_PWM);    // 
  start_encoder_count(5000);
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  Serial.println("motor_stop(0%)");
  motor_stop();    // 0%
  delay(2000);
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  Serial.println("motor_forward(30%)");
  motor_PWM = 30;
  motor_forward(motor_PWM);    // 
  start_encoder_count(5000); 
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  Serial.println("motor_stop(0%)");
  Serial.println("====================================");
  motor_stop();    // 0%
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);
  delay(2000);
  //------------------------------------------------------

  motor_PWM = 15;
  Serial.println("motor_reverse(15%)");
  motor_reverse(motor_PWM);    // 
  start_encoder_count(5000);
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  Serial.println("motor_stop(0%)");
  motor_stop();    // 0%
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);
  delay(2000);

  Serial.println("motor_reverse(20%)");
  motor_PWM = 20;
  motor_reverse(motor_PWM);    // 
  start_encoder_count(5000);
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  Serial.println("motor_stop(0%)");
  motor_stop();    // 0%
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);
  delay(2000);

  Serial.println("motor_reverse(30%)");
  motor_PWM = 30;
  motor_reverse(motor_PWM);    // 
  start_encoder_count(5000); 
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  Serial.println("motor_stop(0%)");
  Serial.println("====================================");
  motor_stop();    // 0%
  Serial.print("A0 : ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);
  delay(2000);
  //------------------------------------------------------


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


