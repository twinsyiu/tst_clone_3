//#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
//#include "EE_24C32.h"
//
//#include "motor_ctlr.h"
//#include "encoder.h"
//#include "UltraSound.h"
//#include <Wire.h>   // use I2C library
//#include <LiquidCrystal_I2C.h> // // use LiquidCrystal_I2C FC-113, install lib required
//
//#define DEBUG 1
//float vcc_volt;
///*
//const byte ults_interrupt_pin = 3;
//int ults_echo_chg_cnt = 0;
//unsigned long echo_hi_ts_us = 0;
//unsigned long echo_lo_ts_us = 0;
//*/
//const int buzzerPin = 9;
//
//// initialize the library with the numbers of the interface pins
//LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
//
//enum movement_st 
//{
//  MVSTATE_STOP,
//  MVSTATE_GO_FWD,
//  MVSTATE_TURN_LEFT,
//  MVSTATE_TURN_RIGHT,
//  MVSTATE_LEFT_90,
//  MVSTATE_RIGHT_90,
//  MVSTATE_TURN_180,
//  MVSTATE_TRAPPED,
//  MVSTATE_KEEP_LEFT,
//  MVSTATE_KEEP_RIGHT
//};
//
//
//
//void setup() 
//{
//  // initialize serial communication at 115200 bits per second:
//  Serial.begin(115200);
//
//  Serial.print("A0 : ");
//  Serial.println(analogRead(A0));
//  
//  Serial.print("A0 : **** ");
//  vcc_volt = analogRead(A0)*5*3/1023;
//  Serial.println(vcc_volt);
//
//  lcd.init(); //initialize the lcd
//  lcd.backlight(); //open the backlight 
//
//  if (vcc_volt < 9.5)
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
//  delay(1000);
//
//  //lcd.init(); //initialize the lcd
//  
//  ultrasound_init( );
//
//  
//  
//  // attachInterrupt(digitalPinToInterrupt(ults_interrupt_pin), ults_echo_chg_isr, CHANGE);
//  
//  // Timer0 is already used for millis() - we'll just interrupt somewhere
//  // in the middle and call the "Compare A" function below
//  OCR0A = 0xAF;
//  TIMSK0 |= _BV(OCIE0A);
//
//  interrupts(); // enable all interrupts
//  buzzerPin_setup();
//  EE_24C32_setup();
//
//  echo_hi_ts_us = 0;
//  echo_lo_ts_us = 0;
//
//  // Now set up speedo Task to run independently.
//  xTaskCreate(
//    Task_Speedo
//    ,  (const portCHAR *)"Task_Speedo"  // A name just for humans
//    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
//    ,  NULL
//    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
//    ,  NULL );
//
//
//
//}
/////* MY NOT USE
//ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
//{
//}
////*/
//
//// MY Interrupt is called once a millisecond, 
//SIGNAL(TIMER0_COMPA_vect) 
//{
//
//  encoder_update();
//
//}
//
////MY float f_dist, l_dist, r_dist, tg_f, tg_l, tg_r;
//float  tg_f, tg_l, tg_r;
//int f_dist, l_dist, r_dist;
//
//void loop()
//{
//  //int ms_ts;
//  //long duration;
////  String stringOne;
//  unsigned int motor_PWM, movement_state, l_motor_PWM, r_motor_PWM;
//  encoder_Struct encoder;
//  int l_faster, prev_l_faster;
//
////  EE_24C32_loop();
////  song_loop();
//
//  int change_state_ms_ts;
//
//  // determine terrain state
//  // if ( f_dist > 30 )
//  movement_state = MVSTATE_STOP;
//  motor_PWM = 20;
//  
//  while (1)
//  {
//  #ifdef DEBUG 
////  Serial.println("motor_stop IMMEDIATELY !!!");
//  Serial.println("Left Motor  "); Serial.print(l_motor_PWM);
//  Serial.println("Right Motor "); Serial.print(r_motor_PWM);
// 
//  #endif
//    switch (movement_state)
//    {
//      case MVSTATE_STOP:
//        //Serial.println(" MVST_STOP");
//        lcd.setCursor(0,0); // set the cursor to column 0, line 0
//        lcd.print("                "); // Print < to the LCD.
//        lcd.setCursor(0,1); // set the cursor to column 0, line 1
//        lcd.print("   0            "); // Print < to the LCD.
//        //display_dist("   0   ");
//        delay(200);
//
//        if ( f_dist > 40 )
//        {
//          //Serial.println("_FWD");
//          movement_state = MVSTATE_GO_FWD;
//          motor_forward( motor_PWM );
//          reset_encoder_count();
//          prev_l_faster = 0;
//          l_motor_PWM = r_motor_PWM = motor_PWM;
//        }
//        else if ( (l_dist > r_dist) && (l_dist > 40) )
//        {
//          //Serial.println("TN_L");
//          movement_state = MVSTATE_TURN_LEFT;
//          change_state_ms_ts = millis();
//          motor_PWM = 16;
//          motor_turn_left( motor_PWM );
//        }
//        else if ( r_dist > 40 )
//        {
//          //Serial.println("TN_R");
//          movement_state = MVSTATE_TURN_RIGHT;
//          change_state_ms_ts = millis();
//          motor_PWM = 16;
//          motor_turn_right( motor_PWM );
//        }
//        else
//        {
//          //Serial.println("TRAP");
//          movement_state = MVSTATE_TRAPPED;
//          change_state_ms_ts = millis();
//          motor_reverse( motor_PWM );
//        }
//        delay(200);
//        break;
//
//      case MVSTATE_GO_FWD:
//        //Serial.println(" MVST_FWD");
//        display_dist("   ^   ");
////        lcd.setCursor(0,1); // set the cursor to column 0, line 1
////        lcd.print("   ^   "); // Print < to the LCD.
//        delay(200);
//
// 
//        if ( f_dist < 40 )
//        {
//          if (( l_dist >= r_dist ) && ( l_dist > 40 ))
//          {
//            tg_r = f_dist;
//            tg_f = l_dist;
//            motor_turn_left( motor_PWM - 5 );
//            movement_state = MVSTATE_LEFT_90;
//          }
//          else if ( r_dist > 40 )
//          {
//            tg_l = f_dist;
//            tg_f = r_dist;
//            motor_turn_right( motor_PWM - 5 );
//            movement_state = MVSTATE_RIGHT_90;
//          }
//          else
//          {
//            tg_l = r_dist;
//            tg_r = l_dist;
//            motor_turn_right( motor_PWM - 5 );
//            movement_state = MVSTATE_RIGHT_90;  // use right turn to achieve the 180 degree turn 
//          }
//          change_state_ms_ts = millis();
//        } 
//        else if ( l_dist < 30 )
//        {
//          //motor_turn_right( motor_PWM );
//          movement_state = MVSTATE_KEEP_LEFT;
//          reset_encoder_count();
//          l_motor_PWM = motor_PWM + 2;
//          r_motor_PWM = motor_PWM - 2;
//          motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
//          motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
//          change_state_ms_ts = millis();
//        } 
//        else if ( r_dist < 30 )
//        {
//          // motor_turn_left( motor_PWM );
//          movement_state = MVSTATE_KEEP_RIGHT;
//          reset_encoder_count();
//          l_motor_PWM = motor_PWM - 2;
//          r_motor_PWM = motor_PWM + 2;
//          motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
//          motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
//          change_state_ms_ts = millis();
//        }
//        delay(200);
//
//        // detect if stucked
//        
//        break;
//
//      //case MVSTATE_TURN_LEFT:
//      case MVSTATE_KEEP_LEFT:
//        //Serial.println(" MVST_KP_L");
//        //lcd.setCursor(0,1); // set the cursor to column 0, line 1
//        //lcd.print("   |>  "); // Print < to the LCD.
//        display_dist("   |>  ");
//        delay(200);
//
//        if ( f_dist < 40 )
//        {
//          motor_stop();
//          movement_state = MVSTATE_STOP;
//          change_state_ms_ts = millis();
//          continue;
//        }
//
//        if ( l_dist > 40 )
//        {
//          l_motor_PWM -= 1;
//          r_motor_PWM += 1;
//          if ( l_motor_PWM < 15 )
//            l_motor_PWM = 15;
//        } else if ( l_dist > 35 )
//        {
//          motor_forward( motor_PWM );
//          l_motor_PWM = r_motor_PWM = motor_PWM;
//          reset_encoder_count();
//          prev_l_faster = 0;
//        } else if ( l_dist < 30 )
//        {
//          l_motor_PWM += 1;
//          r_motor_PWM -= 1;
//          if ( r_motor_PWM < 15 )
//            r_motor_PWM = 15;
//        }
//        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
//        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
//          
//        // detect if stucked
//        delay(200);
//
//        break;
//        
//      //case MVSTATE_TURN_RIGHT:
//      case MVSTATE_KEEP_RIGHT:
//        //Serial.println(" MVST_KP_R");
//        //lcd.setCursor(0,1); // set the cursor to column 0, line 1
//        //lcd.print("  <|   "); // Print < to the LCD.
//        display_dist("  <|   ");
//        delay(200);
//
//        if ( f_dist < 40 )
//        {
//          motor_stop();
//          movement_state = MVSTATE_STOP;
//          change_state_ms_ts = millis();
//          continue;
//        }
//        if ( r_dist > 40 )
//        {
//          r_motor_PWM -= 1;
//          l_motor_PWM += 1;
//          if ( r_motor_PWM < 15 )
//            r_motor_PWM = 15;
//        } else if ( r_dist > 35 )
//        {  
//          motor_forward( motor_PWM );
//          l_motor_PWM = r_motor_PWM = motor_PWM;
//          reset_encoder_count();
//          prev_l_faster = 0;
//          
//        } else if ( r_dist < 30 )
//        {
//          r_motor_PWM += 1;
//          l_motor_PWM -= 1;
//          if ( l_motor_PWM < 15 )
//            l_motor_PWM = 15;
//        }
//        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
//        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
//
//        // detect if stucked
//        delay(200);
//
//        break;
//
//      case MVSTATE_TURN_LEFT:
//      //case 100:
//        //Serial.println(" MVST_TN_L");
//        //lcd.setCursor(0,1); // set the cursor to column 0, line 1
//        //lcd.print("  <    "); // Print < to the LCD.
//        display_dist("  <    ");
//        delay(200);
//
//        if ( f_dist > 40 )
//        {
//          motor_PWM = 20;
//          motor_forward( motor_PWM );
//          movement_state = MVSTATE_GO_FWD;
//          reset_encoder_count();
//          prev_l_faster = 0;
//          l_motor_PWM = r_motor_PWM = motor_PWM;
//          continue;
//        } else if ( ( millis() - change_state_ms_ts ) > 500 )
//        {
//          motor_stop();
//          movement_state = MVSTATE_STOP;
//          change_state_ms_ts = millis();
//          continue;
//        }
//
//        if ( l_dist < 30 )
//        {
//          if ( r_dist < 30 )
//          {
//            movement_state = MVSTATE_TRAPPED;
//            motor_reverse( motor_PWM );
//            change_state_ms_ts = millis();
//            continue;
//          }
//          movement_state = MVSTATE_TURN_RIGHT;
//          change_state_ms_ts = millis();
//          motor_PWM = 16;
//          motor_turn_right( motor_PWM );
//          
//        }
//        break;
//
//      //case 101:
//      case MVSTATE_TURN_RIGHT:
//        //Serial.println(" MVST_TN_R");
//        //lcd.setCursor(0,1); // set the cursor to column 0, line 1
//        //lcd.print("    >  "); // Print < to the LCD.
//        display_dist("    >  ");
//        delay(200);
//
//        if ( f_dist > 40 )
//        {
//          motor_PWM = 20;
//          motor_forward( motor_PWM );
//          movement_state = MVSTATE_GO_FWD;
//          reset_encoder_count();
//          prev_l_faster = 0;
//          l_motor_PWM = r_motor_PWM = motor_PWM;
//          continue;
//        } else if ( ( millis() - change_state_ms_ts ) > 500 )
//        {
//          motor_stop();
//          movement_state = MVSTATE_STOP;
//          change_state_ms_ts = millis();
//          continue;
//        }
//
//        if ( r_dist < 30 )
//        {
//          if ( l_dist < 30 )
//          {
//            movement_state = MVSTATE_TRAPPED;
//            motor_reverse( motor_PWM );
//            change_state_ms_ts = millis();
//            continue;
//          }
//          movement_state = MVSTATE_TURN_LEFT;
//          change_state_ms_ts = millis();
//          motor_PWM = 16;
//          motor_turn_left( motor_PWM );
//          
//        }
//        break;
// 
//      case MVSTATE_TRAPPED:
//      default:
//        //Serial.println("MVST_TRAP");
//        //lcd.setCursor(0,1); // set the cursor to column 0, line 1
//        //lcd.print(" ||=|| "); // Print < to the LCD.
//        display_dist(" ||=|| ");
//        
//        delay(200);
//       if ( l_dist > 30 )
//        {
//          change_state_ms_ts = millis();
//          movement_state = MVSTATE_TURN_LEFT;
//          motor_PWM = 16;
//          motor_turn_left( motor_PWM );
//          continue;
//        }
//        if ( r_dist > 30 )
//        {
//          change_state_ms_ts = millis();
//          movement_state = MVSTATE_TURN_RIGHT;
//          motor_PWM = 16;
//          motor_turn_right( motor_PWM );
//          continue;
//          
//        }
//
//        break;
//    }
//  }
//}
//
// 
//
// 
//// PWM_adjust() will bring the l_PWM & r_PWM back to the PWM_centre when there is small diff between the l & r PWM
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
//}
//
//float ultr_dist[3];
//
// 
//
//void display_dist (char * dir_str)
//{
//  lcd.setCursor(0,0); // set the cursor to column 0, line 0
//  lcd.print("L"); 
//  lcd.setCursor(1,0); // set the cursor to column 0, line 0
//  lcd.print(l_dist); // Print l_dist to the LCD.
//
//  lcd.setCursor(6,0); // set the cursor to column 0, line 0
//  lcd.print("F"); 
//  lcd.setCursor(7,0); // set the cursor to column 5, line 0
//  lcd.print(f_dist); // Print l_dist to the LCD.
//
//  lcd.setCursor(12,0); // set the cursor to column 0, line 0
//  lcd.print("R");  
//  lcd.setCursor(13,0); // set the cursor to column 10, line 0
//  lcd.print(r_dist); // Print l_dist to the LCD.
//  
//  lcd.setCursor(0,1); // set the cursor to column 0, line 1
//  lcd.print(dir_str); // Print < to the LCD.
//
//  
//}
