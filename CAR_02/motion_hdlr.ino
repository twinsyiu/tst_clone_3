#include "encoder.h"

const int action_delay = 200;

unsigned long current_time;
unsigned long next_mtr_state_time_ms = 0;
unsigned long next_motion_stuck_time_ms = 0;

void init_motion_handler(void)
{
  movement_state = MVSTATE_STOP;
  motor_PWM = 25;
  motor_stop();
  l_motor_PWM = r_motor_PWM = motor_PWM;
}

/*
 * motion_stuck_handler() will be activated on every second.
 * It will check if the motors are running before taking actions.
 * Motor is considered stucked, if either of the wheel encoder returns less than 5 slots changes in the last one second.
 * The function will choose the opposite direction as the primary response if motor stucked is found.
 * If frequent motor stucked conditions (previous stuck are less than 2 seconds) are found 
 * then special treatment, and delay() is used as there is no better choice until the stuck condition is cleared 
 */
void motion_stuck_handler(void)
{
  unsigned long current_time;
  encoder_Struct encoder;
  static unsigned long last_motion_stuck_time;
  
  current_time = millis();
  if ( current_time < next_motion_stuck_time_ms )
    return;
  next_motion_stuck_time_ms = current_time + 1000;    // per second motion stuck check
  get_encoder(&encoder);

  switch (movement_state)
  {
    case MVSTATE_STOP:
    case MVSTATE_DEAD_STOP:
      // do nothing if motor is in either one of these two state.
      return;
    default:
      break;
  }

  if ( encoder.L_E_count_1s < 5 or encoder.R_E_count_1s < 5 )
  {
    switch (movement_state)
    {
      case MVSTATE_GO_FWD:
      case MVSTATE_KEEP_LEFT:
      case MVSTATE_KEEP_RIGHT:
        if (( current_time - last_motion_stuck_time ) > 2000 )
        {
          motor_reverse(motor_PWM * 0.8);
          l_motor_PWM = r_motor_PWM = motor_PWM * 0.8;
          movement_state = MVSTATE_REV;
          last_motion_stuck_time = current_time;
          break;
        }
        // its requent stuck, special treatment to be executed
        display_dist(" <^>S");
        // else it has been stuck from previous opposite motion
        motor_reverse(motor_PWM * 0.8);
        movement_state = MVSTATE_REV;
        delay(500); // allow room to turn
        motor_turn_left(motor_PWM * 0.6);
        movement_state = MVSTATE_TURN_LEFT;
        delay(500); // make sure enough turn
        l_motor_PWM = r_motor_PWM = motor_PWM;
        motor_forward(motor_PWM);
        movement_state = MVSTATE_GO_FWD;
        last_motion_stuck_time = current_time;
        break;

      case MVSTATE_REV:
        if (( current_time - last_motion_stuck_time ) > 2000 )
        {
          motor_forward(motor_PWM * 0.8);
          l_motor_PWM = r_motor_PWM = motor_PWM * 0.8;
          movement_state = MVSTATE_GO_FWD;
          last_motion_stuck_time = current_time;
          break;
        }
        // its requent stuck, special treatment to be executed
        display_dist("  V S");
        // else it has been stuck from previous opposite motion
        motor_forward(motor_PWM * 0.8);
        movement_state = MVSTATE_GO_FWD;
        delay(500); // allow room to turn
        motor_turn_left(motor_PWM * 0.6);
        movement_state = MVSTATE_TURN_LEFT;
        delay(500); // make sure enough turn
        l_motor_PWM = r_motor_PWM = motor_PWM;
        motor_reverse(motor_PWM);
        movement_state = MVSTATE_REV;
        last_motion_stuck_time = current_time;
        break;

      case MVSTATE_TURN_RIGHT:
        display_dist("  > S");
        motor_reverse(motor_PWM * 0.8);
        movement_state = MVSTATE_REV;
        delay(500); // allow room to turn
        motor_turn_left(motor_PWM * 0.6);
        movement_state = MVSTATE_TURN_LEFT;
        delay(500); // make sure enough turn
        motor_forward(motor_PWM);
        l_motor_PWM = r_motor_PWM = motor_PWM;
        movement_state = MVSTATE_GO_FWD;
        last_motion_stuck_time = current_time;
        break;

      case MVSTATE_TURN_LEFT:
        display_dist("S <  ");
        motor_reverse(motor_PWM * 0.8);
        movement_state = MVSTATE_REV;
        delay(500); // allow room to turn
        motor_turn_right(motor_PWM * 0.6);
        movement_state = MVSTATE_TURN_LEFT;
        delay(500); // make sure enough turn
        motor_forward(motor_PWM);
        l_motor_PWM = r_motor_PWM = motor_PWM;
        movement_state = MVSTATE_GO_FWD;
        last_motion_stuck_time = current_time;
        break;

      
      default:
        break;
    }
  }
}

void motion_handler(void)
{
  unsigned long current_time;
  
  current_time = millis();
  if ( current_time < next_mtr_state_time_ms )
    return;
  next_mtr_state_time_ms = current_time + action_delay;
//  Serial.print(current_time);Serial.print(" -- ");Serial.println(next_mtr_state_time_ms);

  switch (movement_state)
  {
    case MVSTATE_TURN_RIGHT:
      I2C_RGB_LED(R_BL);
      goto L_R_TURN;
    case MVSTATE_TURN_LEFT:
      I2C_RGB_LED(L_BL);
L_R_TURN:
      if ( f_dist < 40 )
      {
        // adjust the wick up time
        next_mtr_state_time_ms = current_time + 100;
        return;
      }
      // else f_dist >= 40      
      movement_state = MVSTATE_GO_FWD;
      motor_forward( motor_PWM );
      break;

    case MVSTATE_REV:
      I2C_RGB_LED(L_BL_R_BL);
      if ( f_dist < 40 || ( l_dist < 35 && r_dist < 35 ))
      {
        // motor_reverse( motor_PWM );
        return;
      } 
      // i.e. else ( f_dist >= 40 && ( l_dist >= 35 || r_dist >= 35 ))

      if ( r_dist > 35 )
      {
        //Turn Right 100ms
        movement_state = MVSTATE_TURN_RIGHT;
        motor_turn_right( motor_PWM*0.8);
        // adjust the wick up time
        // next_mtr_state_time_ms = current_time + 100;
      }
      else
      {
        //Turn Left 100ms
        movement_state = MVSTATE_TURN_LEFT;
        motor_turn_left( motor_PWM*0.8);
        // adjust the wick up time
        // next_mtr_state_time_ms = current_time + 100;
      }  
      break;

    case MVSTATE_STOP:
      I2C_RGB_LED(ALL_OFF);
      l_motor_PWM = r_motor_PWM = motor_PWM; //MY for display_dist

      if ( f_dist < 40 || ( l_dist < 35 && r_dist < 35 ) )
      {
        movement_state = MVSTATE_REV;
        motor_reverse( motor_PWM );
        return;
      }
        
      if ( f_dist > 40)
      {
        movement_state = MVSTATE_GO_FWD;
        motor_forward( motor_PWM );
      }
      if ( r_dist > 35 )
      {
        //Turn Right 100ms
        movement_state = MVSTATE_TURN_RIGHT;
        motor_turn_right( motor_PWM*0.8);
        // adjust the wick up time
        // next_mtr_state_time_ms = current_time + 100;
      }
      else  // if ( l_dist > 35 )
      {
        //Turn Left 100ms
        movement_state = MVSTATE_TURN_LEFT;
        motor_turn_left( motor_PWM*0.8);
        // adjust the wick up time
        // next_mtr_state_time_ms = current_time + 100;
      }  

      break;

    case MVSTATE_GO_FWD:
      I2C_RGB_LED(L_WH_R_WH);
      FWD_straight_adj();
      
      if ( f_dist < 35 || ( l_dist < 25 && r_dist < 25 ) )
      {
        movement_state = MVSTATE_STOP;
        motor_stop();
        return;
      }

      // 
      if (l_dist < 35 && r_dist < 35 )
      { // reduce the forward speed if both left and right distance are less than 35,  ie running in a logn corridor
        l_motor_PWM = r_motor_PWM = motor_PWM * 0.8; //MY for display_dist
        motor_forward( motor_PWM * 0.8 );
      }
      else if ( l_dist < 35 )
      { // i.e. only the left side is closing to a wall
        // Keeping Right
        movement_state = MVSTATE_KEEP_RIGHT;
      }
      else if ( r_dist < 35 )
      { // i.e. only the right side is closing to a wall
        //Keeping Left
        movement_state = MVSTATE_KEEP_LEFT;
      }            
      break;
        
    case MVSTATE_KEEP_LEFT: // due to r_dist < 35
      I2C_RGB_LED(L_RD);
      if ( f_dist < 35)
      {
        movement_state = MVSTATE_STOP;
        motor_stop();
        return;
      }

      //Motor drive to LEFT
      r_motor_PWM = motor_PWM - 3;    // slow down the right
      l_motor_PWM = motor_PWM - 8;    // further slow down the left
      motor_chg_dir(  l_motor_PWM,  r_motor_PWM );

      if ( r_dist > 35 )
      {
        l_motor_PWM = r_motor_PWM = motor_PWM;
        motor_forward( motor_PWM );
        movement_state = MVSTATE_GO_FWD;
      }
      break;
        
    case MVSTATE_KEEP_RIGHT: //due to l_dist < 35
      I2C_RGB_LED(R_RD);
      if ( f_dist < 35)
      {
        movement_state = MVSTATE_STOP;
        motor_stop();
        return;
      }

      //Motor drive to RIGHT
      l_motor_PWM = motor_PWM - 3;    // slow down the left
      r_motor_PWM = motor_PWM - 8;    // further slow down the right
      motor_chg_dir(  l_motor_PWM,  r_motor_PWM );

      if ( l_dist > 35 )
      {
        l_motor_PWM = r_motor_PWM = motor_PWM;
        motor_forward( motor_PWM );
        movement_state = MVSTATE_GO_FWD;
      }
      break;
        
    case MVSTATE_DEAD_STOP:  
      I2C_RGB_LED(ALL_OFF);
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



// previous TY's motion control loop
// obsoleted
/*
void loop()
{
  
  unsigned long current_time;
  unsigned long next_mtr_state_time_ms = 0;
  
  movement_state = MVSTATE_STOP;
  motor_PWM = 20;
  
  while (1)
  {

    dislplay_to_LCD();
    motion_handler();
    

  current_time = millis();
  if ( current_time > next_mtr_state_time_ms )
  {
    next_mtr_state_time_ms = current_time + action_delay;
    Serial.print(current_time);Serial.print(" -- ");Serial.println(next_mtr_state_time_ms);
    
    
    switch (movement_state)
    {
      case MVSTATE_STOP:
        Serial.println(" MVST_STOP");
        MVSTATE_STOP_Hdlr();
        break;

      case MVSTATE_GO_FWD:
        GO_FWD_straight_line_Hdlr();

        // PWM balancing between the l & r motor PWM
        PWM_adjust (&l_motor_PWM, &r_motor_PWM, 5, motor_PWM);

        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );

        // remember the l_faster status for next calculation
        prev_l_faster = l_faster;

        // detect if stucked
        if (is_stuck)
        {
          movement_state = MVSTATE_TRAPPED;
          change_state_ms_ts = millis();
          motor_reverse( motor_PWM );
          break;
        }

        // check if it is blocked by any object
        // =============================================
        if ( f_dist < 40 )
        {
          if (( l_dist >= r_dist ) && ( l_dist > 40 ))
          {
            motor_turn_left( motor_PWM - 5 );
            movement_state = MVSTATE_TURN_LEFT;
            init_wheel_stuck_check();
          }
          else if ( r_dist > 40 )
          {
            motor_turn_right( motor_PWM - 5 );
            movement_state = MVSTATE_TURN_RIGHT;
            init_wheel_stuck_check();
          }
          else
          {
            movement_state = MVSTATE_TRAPPED;
            change_state_ms_ts = millis();
            motor_reverse( motor_PWM );
          }
          change_state_ms_ts = millis();
        } 
        else if ( l_dist < 30 )
        {
          //motor_turn_right( motor_PWM );
          movement_state = MVSTATE_KEEP_LEFT;
          reset_encoder_count();
          l_motor_PWM = motor_PWM + 2;
          r_motor_PWM = motor_PWM - 2;
          motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
          motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
          change_state_ms_ts = millis();
        } 
        else if ( r_dist < 30 )
        {
          // motor_turn_left( motor_PWM );
          movement_state = MVSTATE_KEEP_RIGHT;
          reset_encoder_count();
          l_motor_PWM = motor_PWM - 2;
          r_motor_PWM = motor_PWM + 2;
          motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
          motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
          change_state_ms_ts = millis();
        }
        
        break;

      case MVSTATE_KEEP_LEFT:
        if ( f_dist < 40 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          break;
        }

        if ( l_dist > 40 )
        {
          // go forward
          movement_state = MVSTATE_GO_FWD;
          motor_forward( motor_PWM );
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
          
          init_wheel_stuck_check();
        }
        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
          
        // detect if stucked
        if (is_stuck)
        {
          movement_state = MVSTATE_TRAPPED;
          change_state_ms_ts = millis();
          motor_reverse( motor_PWM );
          break;
        }

        break;
        
      case MVSTATE_KEEP_RIGHT:
        //display_dist("  <|   ");
        //delay(lcd_delay);

        if ( f_dist < 40 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          break;
        }
        
        if ( r_dist > 40 )
        {
          // go forward
          movement_state = MVSTATE_GO_FWD;
          motor_forward( motor_PWM );
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;

          init_wheel_stuck_check();
        }
        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );

        // detect if stucked
        if (is_stuck)
        {
          movement_state = MVSTATE_TRAPPED;
          change_state_ms_ts = millis();
          motor_reverse( motor_PWM );
          break;
        }

        break;

      case MVSTATE_TURN_LEFT:
        //display_dist("  <    ");
        //delay(lcd_delay);

        if ( f_dist > 40 )
        {
          motor_PWM = 20;
          motor_forward( motor_PWM );
          movement_state = MVSTATE_GO_FWD;
          reset_encoder_count();
          init_wheel_stuck_check();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
          break;
        } else if ( ( millis() - change_state_ms_ts ) > 500 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          break;
        }

        if ( l_dist < 30 )
        {
          movement_state = MVSTATE_TRAPPED;
          motor_reverse( motor_PWM );
          change_state_ms_ts = millis();
          break;
        }

        // detect if stucked
        if (is_stuck)
        {
          movement_state = MVSTATE_TRAPPED;
          change_state_ms_ts = millis();
          motor_reverse( motor_PWM );
          break;
        }
        break;

      case MVSTATE_TURN_RIGHT:
        if ( f_dist > 40 )
        {
          motor_PWM = 20;
          motor_forward( motor_PWM );
          movement_state = MVSTATE_GO_FWD;
          reset_encoder_count();
          init_wheel_stuck_check();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
          break;
        } else if ( ( millis() - change_state_ms_ts ) > 500 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          break;
        }

        if ( r_dist < 30 )
        {
          if ( l_dist < 30 )
          {
            movement_state = MVSTATE_TRAPPED;
            motor_reverse( motor_PWM );
            change_state_ms_ts = millis();
            break;
          }
          movement_state = MVSTATE_TURN_LEFT;
          change_state_ms_ts = millis();
          motor_PWM = 16;
          motor_turn_left( motor_PWM );
          reset_encoder_count();
        }
         // detect if stucked
        if (is_stuck)
        {
          movement_state = MVSTATE_TRAPPED;
          change_state_ms_ts = millis();
          motor_reverse( motor_PWM );
          break;
        }
       break;
        
      case MVSTATE_TRAPPED:
        delay(2000);
        if ( (l_dist > 40) || (r_dist > 40) )
        {
          motor_stop();
          change_state_ms_ts = millis();
          movement_state = MVSTATE_STOP;
        }

        break;
      default:
        display_dist("#######");
        break;
    }
  }
  }
}

*/

