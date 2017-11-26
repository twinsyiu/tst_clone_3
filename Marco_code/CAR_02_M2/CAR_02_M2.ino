#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
//#include "EE_24C32.h"

#include "motor_ctlr.h"
#include "encoder.h"
#include "UltraSound.h"
#include <Wire.h>   // use I2C library
#include <LiquidCrystal_I2C.h> // // use LiquidCrystal_I2C FC-113, install lib required

//#define DEBUG 1 //triger the DEBUG Serial.print();
float vcc_volt;
/*
const byte ults_interrupt_pin = 3;
int ults_echo_chg_cnt = 0;
unsigned long echo_hi_ts_us = 0;
unsigned long echo_lo_ts_us = 0;
*/
//const int buzzerPin = 9;

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

enum movement_st 
{
  MVSTATE_STOP,
  MVSTATE_GO_FWD,
  MVSTATE_TURN_LEFT,
  MVSTATE_TURN_RIGHT,
  MVSTATE_LEFT_90,
  MVSTATE_RIGHT_90,
  MVSTATE_TURN_180,
  MVSTATE_TRAPPED,
  MVSTATE_KEEP_LEFT,
  MVSTATE_KEEP_RIGHT,
  MVSTATE_DEAD_STOP //MY for low voltage
};

//void check_volt(int)
//{
//if (vcc_volt < 9.5)
//  {
//    Serial.print("LOW POWER SUPPLY VOLTAGE !!! PLEASE RECHARGE BATTERY NOW ");
//    lcd.setCursor(0,0); // set the cursor to column 0, line 0
//    lcd.print("BATTERY LOW"); // Print l_dist to the LCD.
//    lcd.setCursor(0,1); // set the cursor to column 0, line 0
//    lcd.print(vcc_volt); // Print l_dist to the LCD.
//    lcd.setCursor(6,1); // set the cursor to column 0, line 0
//    lcd.print("Volt"); // Print l_dist to the LCD.
////    while (1) {};
//  }
//  else
//  {
//    lcd.setCursor(0,0); // set the cursor to column 0, line 0
//    lcd.print("MOTOR DISABLE TST"); // Print l_dist to the LCD.
//    lcd.setCursor(0,1); // set the cursor to column 0, line 0
//    lcd.print(vcc_volt); // Print l_dist to the LCD.
//    lcd.setCursor(6,1); // set the cursor to column 0, line 0
//    lcd.print("Volt"); // Print l_dist to the LCD.
//  }  
//}

void setup() 
{
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  Serial.print("A0 : ");
  Serial.println(analogRead(A0));
  
  Serial.print("A0 : **** ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 
//  check_volt();
  if (vcc_volt < 9.5)
  {
    Serial.print("LOW POWER SUPPLY VOLTAGE !!! PLEASE RECHARGE BATTERY NOW ");
    lcd.setCursor(0,0); // set the cursor to column 0, line 0
    lcd.print("BATTERY LOW"); // Print l_dist to the LCD.
    lcd.setCursor(0,1); // set the cursor to column 0, line 0
    lcd.print(vcc_volt); // Print l_dist to the LCD.
    lcd.setCursor(6,1); // set the cursor to column 0, line 0
    lcd.print("Volt"); // Print l_dist to the LCD.
//    while (1) {};
  }
  else
  {
    lcd.setCursor(0,0); // set the cursor to column 0, line 0
    lcd.print("MOTOR DISABLE TST"); // Print l_dist to the LCD.
    lcd.setCursor(0,1); // set the cursor to column 0, line 0
    lcd.print(vcc_volt); // Print l_dist to the LCD.
    lcd.setCursor(6,1); // set the cursor to column 0, line 0
    lcd.print("Volt"); // Print l_dist to the LCD.
  }
  delay(1000);

  //lcd.init(); //initialize the lcd
  
  ultrasound_init( );

  
  
  // attachInterrupt(digitalPinToInterrupt(ults_interrupt_pin), ults_echo_chg_isr, CHANGE);
  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  interrupts(); // enable all interrupts
//  buzzerPin_setup();
//  EE_24C32_setup();

  echo_hi_ts_us = 0;
  echo_lo_ts_us = 0;

  // Now set up speedo Task to run independently.
  xTaskCreate(
    Task_Speedo
    ,  (const portCHAR *)"Task_Speedo"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );



}
///* MY NOT USE
ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
}
//*/

// MY Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) 
{
//encoder_Struct encoder;
  encoder_update();
//Serial.print("l_encoder_count= ");Serial.print(encoder.l_encoder_count);Serial.print(" r_encoder_count= ");Serial.println(encoder.r_encoder_count);
}

//MY float f_dist, l_dist, r_dist, tg_f, tg_l, tg_r;
float  tg_f, tg_l, tg_r;
unsigned int f_dist, l_dist, r_dist; //MY
unsigned int motor_PWM, movement_state, l_motor_PWM, r_motor_PWM; //MY

void loop()
{
  //int ms_ts;
  //long duration;
//  String stringOne;
//MY  unsigned int motor_PWM, movement_state, l_motor_PWM, r_motor_PWM;
//  encoder_Struct encoder;
  int l_faster, prev_l_faster;

//  EE_24C32_loop();
//  song_loop();

//MY  int change_state_ms_ts;

  // determine terrain state
  // if ( f_dist > 25 )
//MY  movement_state = MVSTATE_STOP;
  movement_state = MVSTATE_GO_FWD; //MY
  motor_PWM = 25;
   motor_forward( motor_PWM );
  l_motor_PWM = r_motor_PWM = motor_PWM; //MY
  while (1)
  {
  #ifdef DEBUG 
  Serial.print("MVSTATE= ");Serial.println(movement_state);
  Serial.print("Left Motor PWM= "); Serial.println(l_motor_PWM);
  Serial.print("Right Motor PWM="); Serial.println(r_motor_PWM);
  Serial.print("l_dist= "); Serial.print(l_dist);Serial.print(" f_dist= ");Serial.print(f_dist);Serial.print(" r_dist= ");Serial.println(r_dist);
  #endif
  Serial.print("loop movement_state= ");Serial.println(movement_state);
  switch (movement_state)
  {
    case MVSTATE_STOP:
 
        display_dist("  0  ");
        delay(200);

        l_motor_PWM = r_motor_PWM = motor_PWM; //MY for display_dist
        while ( f_dist < 40 ) // stop loop f_dist > 50
        {
          motor_reverse( motor_PWM );
          display_dist(" 0-RV"  );
        }
        if ( l_dist < 35 && r_dist < 35)
        {
          while ( l_dist < 35 && r_dist < 35 ) // stop loop l_dist & r_dist >35
          {        
            motor_reverse (motor_PWM);
            display_dist(" 0-RV");
            // Then ?
          }
          if (l_dist < 35 )
          {
            //Turn Right
            motor_turn_right( motor_PWM*0.8);
            display_dist(" 0->>");
            delay (100); //time allow to turn
              motor_forward( motor_PWM );
          }
          else if ( r_dist < 35 )
          {
            //Turn Left
            motor_turn_left( motor_PWM*0.8);
            display_dist(" 0-<<");            
            delay (100); //time allow to turn
              motor_forward( motor_PWM );
          }  
        }
        else
        {
          motor_turn_left( motor_PWM*0.8);
          display_dist(" 0-<<");
          delay (100); //time allow to turn
          motor_forward( motor_PWM );
        }
        if ( f_dist > 40)
        {
          movement_state = MVSTATE_GO_FWD;
        }
        break;
        
    case MVSTATE_GO_FWD:
        display_dist("  ^  ");
        delay(200);
        l_motor_PWM = r_motor_PWM = motor_PWM; //MY for display_dist
          motor_forward( motor_PWM );
        FWD_straight_adj();
        if ( f_dist < 35)
        {
          movement_state = MVSTATE_STOP;
        }
        else
        {
          // Serial.println(" else "); 
          if ( l_dist && r_dist <25)
          {
           // Serial.println(" <<25 ");
          movement_state = MVSTATE_STOP;
          }
          else if (l_dist < 35 && r_dist <35)
          {
            // Serial.println(" <<35 "); 
            //Reduce Speed to 80% 
              motor_forward( motor_PWM*0.8 );
          }
          else if ( l_dist < 35 )
          {
            //Keeping Right
            movement_state = MVSTATE_KEEP_RIGHT;
          }
          else if ( r_dist < 35 )
          {
            // Serial.print(" r_dist= ");Serial.println(r_dist);
            //Keeping Left
            movement_state = MVSTATE_KEEP_LEFT;
          }            
        }
        break;
        
      case MVSTATE_KEEP_LEFT: // due to r_dist < 35
        display_dist(" |> ");
        delay(200);
        if ( f_dist < 35)
        {
          movement_state = MVSTATE_STOP;
        }
        else
        {
          l_motor_PWM = r_motor_PWM = motor_PWM;
          while ( r_dist < 35  ) // loop stop as (r_dist < 35) is flase
          {
            display_dist(" |> ");
            //Motor drive to LEFT
            l_motor_PWM = motor_PWM - 8;
            r_motor_PWM = motor_PWM - 3;
            motor_chg_dir(  l_motor_PWM,  r_motor_PWM );
//            delay(1000);
            if ( l_dist < 35 or f_dist < 35 )
            {
//              movement_state = MVSTATE_GO_FWD;  
              break;   
            }
          }
          movement_state = MVSTATE_GO_FWD;
        }
        break;
        
    case MVSTATE_KEEP_RIGHT: //due to l_dist < 35
        display_dist(" <| ");
        delay(200);
        if ( f_dist < 35)
        {
          movement_state = MVSTATE_STOP;
        }
        else
        {
          l_motor_PWM = r_motor_PWM = motor_PWM;
          while ( l_dist < 35  )  // loop stop as (l_dist < 35) is flase
          {  
            display_dist(" <| ");            
            //Motor drive to RIGHT
            l_motor_PWM = motor_PWM - 3;
            r_motor_PWM = motor_PWM - 8;
            motor_chg_dir(  l_motor_PWM,  r_motor_PWM );
            if ( r_dist < 35 or f_dist < 35 )
            {
              break;     
            }
          }
          movement_state = MVSTATE_GO_FWD;
        }
        break;
        
    case MVSTATE_DEAD_STOP:  
        lcd.setCursor(0,0); // set the cursor to column 0, line 0
        lcd.print("LOW VOLTAGE<9.5V"); // Print < to the LCD.
//        lcd.setCursor(0,14); // set the cursor to column 0, line 0
//        lcd.print(analogRead(A0)*5*3/1023); // Print < to the LCD.        
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print(" MOTOR DEAD STOP"); // Print < to the LCD.
        motor_stop();
        break;    
    }
  
  }

}


 
//MY NOT USE// PWM_adjust() will bring the l_PWM & r_PWM back to the PWM_centre when there is small diff between the l & r PWM
//void PWM_adjust(unsigned int * l_PWM, unsigned int * r_PWM, unsigned int PWM_diff_margin, unsigned int PWM_centre)
//{
//  // if the diff between l_PWM and R_PWM is less than PWM_diff_margin, 
//  // then l_PWM will be set to PWM_centre and r_PWM will be adjusted with the diff
//  unsigned int l_r_diff;
//  
//  if ( *l_PWM >= *r_PWM )
//  {
//    l_r_diff = *l_PWM - *r_PWM;
//    if ( (l_r_diff < PWM_diff_margin) && (*l_PWM != PWM_centre) )
//    {
//      *l_PWM = PWM_centre;
//      *r_PWM = PWM_centre - l_r_diff;
//    }
//  }
//  else
//  {
//    l_r_diff = *r_PWM - *l_PWM;
//    if ( (l_r_diff < PWM_diff_margin) && (*l_PWM != PWM_centre) )
//    {
//      *l_PWM = PWM_centre;
//      *r_PWM = PWM_centre + l_r_diff;
//    }
//    
//  }
//MY}

float ultr_dist[3];

 

void display_dist (char * dir_str)
{
  //MY reset display 
  lcd.setCursor(0,0); // set the cursor to column 0, line 0
  lcd.print("                "); // Print < to the LCD.
  lcd.setCursor(0,1); // set the cursor to column 0, line 1
  lcd.print("                "); // Print < to the LCD.
  
  lcd.setCursor(0,0); // set the cursor to column 0, line 0
  lcd.print("L"); 
  lcd.setCursor(1,0); // set the cursor to column 0, line 0
  lcd.print(l_dist); // Print l_dist to the LCD.

  lcd.setCursor(6,0); // set the cursor to column 0, line 0
  lcd.print("F"); 
  lcd.setCursor(7,0); // set the cursor to column 5, line 0
  lcd.print(f_dist); // Print l_dist to the LCD.

  lcd.setCursor(12,0); // set the cursor to column 0, line 0
  lcd.print("R");  
  lcd.setCursor(13,0); // set the cursor to column 10, line 0
  lcd.print(r_dist); // Print l_dist to the LCD.
  
  lcd.setCursor(0,1); // set the cursor to column 0, line 1
  lcd.print(dir_str); // Print < to the LCD.

  lcd.setCursor(7,1); // set the cursor to column 0, line 1
  lcd.print("LM"); // Print < to the LCD.
  lcd.setCursor(9,1); // set the cursor to column 0, line 1
  lcd.print(l_motor_PWM); // Print < to the LCD.
  lcd.setCursor(12,1); // set the cursor to column 0, line 1
  lcd.print("RM"); // Print < to the LCD.
  lcd.setCursor(14,1); // set the cursor to column 0, line 1
  lcd.print(r_motor_PWM); // Print < to the LCD.

  
}
