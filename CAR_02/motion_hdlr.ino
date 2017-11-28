#include "encoder.h"

const int action_delay = 200;

unsigned long current_time;
unsigned long next_mtr_state_time_ms = 0;
unsigned long next_motion_stuck_time_ms = 0;
unsigned int prev_movement_state;


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

  // if either one wheel has less than 5 slots changes, then it is stucked
  if ( encoder.L_E_count_1s < 5 or encoder.R_E_count_1s < 5 )
  {
    switch (movement_state)
    {
      case MVSTATE_GO_FWD:
      case MVSTATE_KEEP_LEFT:
      case MVSTATE_KEEP_RIGHT:
        
        if (( current_time - last_motion_stuck_time ) > 2000 )
        { // if previous stuck was older than 2 seconds, ie a non-frequent stuck situation, just do a simple reverse
          motor_reverse(motor_PWM * 0.8);
          l_motor_PWM = r_motor_PWM = motor_PWM * 0.8;
          movement_state = MVSTATE_REV;
          last_motion_stuck_time = current_time;
          break;    // this break the switch statement and skip the rest
        }
        // else it is a frequent stuck, previous stuck is within 2 seconds old, special treatment to be executed
        display_dist(" <^>S");

        motor_reverse(motor_PWM * 0.8);
        movement_state = MVSTATE_REV;
        delay(500); // delay to ensure time to move

        if ( movement_state == MVSTATE_KEEP_LEFT )
        {
          motor_turn_right(motor_PWM * 0.6);
          movement_state = MVSTATE_TURN_RIGHT;
        }
        else
        {
          motor_turn_left(motor_PWM * 0.6);
          movement_state = MVSTATE_TURN_LEFT;
        }
        delay(500); // delay to ensure time to turn
        l_motor_PWM = r_motor_PWM = motor_PWM;

        // resume forward mode
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
        // else it is a frequent stuck, special treatment to be executed
        display_dist("  V S");
        // else it has been stuck from previous opposite motion
        motor_forward(motor_PWM * 0.8);
        movement_state = MVSTATE_GO_FWD;
        delay(500); // delay to ensure time to move
        motor_turn_left(motor_PWM * 0.6);
        movement_state = MVSTATE_TURN_LEFT;
        delay(500); // delay to ensure time to turn
        l_motor_PWM = r_motor_PWM = motor_PWM;
        
        // resume reverse mode
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
    case MVSTATE_TURN_LEFT:
      if ( f_dist < 80 )
      {
        // speed up the wake up time
        next_mtr_state_time_ms = current_time + 300;
        return;
      }
      // else f_dist >= 80      
      movement_state = MVSTATE_GO_FWD;
      motor_forward( motor_PWM );
      break;

    case MVSTATE_REV:
      if ( f_dist < 80 || ( l_dist < 35 && r_dist < 35 ))
      {
        return;
      } 
      // i.e. else ( f_dist >= 80 && ( l_dist >= 35 || r_dist >= 35 ))

      if ( r_dist > 35 )
      {
        //Turn Right 100ms
        movement_state = MVSTATE_TURN_RIGHT;
        motor_turn_right( motor_PWM*0.8);
      }
      else
      {
        //Turn Left 100ms
        movement_state = MVSTATE_TURN_LEFT;
        motor_turn_left( motor_PWM*0.8);
      }  
      break;

    case MVSTATE_STOP:
      l_motor_PWM = r_motor_PWM = motor_PWM; //MY for display_dist

      if ( f_dist < 50 || ( l_dist < 35 && r_dist < 35 ) )
      {
        movement_state = MVSTATE_REV;
        motor_reverse( motor_PWM );
        return;
      }
        
      if ( f_dist > 50)
      {
        movement_state = MVSTATE_GO_FWD;
        motor_forward( motor_PWM );
      }
      if ( r_dist > 35 )
      {
        //Turn Right 100ms
        movement_state = MVSTATE_TURN_RIGHT;
        motor_turn_right( motor_PWM*0.8);
      }
      else  // if ( l_dist > 35 )
      {
        //Turn Left 100ms
        movement_state = MVSTATE_TURN_LEFT;
        motor_turn_left( motor_PWM*0.8);
      }  

      break;

    case MVSTATE_GO_FWD:
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

        //Motor drive to RIGHT
        l_motor_PWM = motor_PWM - 3;    // slow down the left
        r_motor_PWM = motor_PWM - 8;    // further slow down the right
        motor_chg_dir(  l_motor_PWM,  r_motor_PWM );
      }
      else if ( r_dist < 35 )
      { // i.e. only the right side is closing to a wall
        //Keeping Left
        movement_state = MVSTATE_KEEP_LEFT;

        //Motor drive to LEFT
        r_motor_PWM = motor_PWM - 3;    // slow down the right
        l_motor_PWM = motor_PWM - 8;    // further slow down the left
        motor_chg_dir(  l_motor_PWM,  r_motor_PWM );
      }            
      break;
        
    case MVSTATE_KEEP_LEFT: // due to r_dist < 35
      if ( f_dist < 35)
      {
        movement_state = MVSTATE_STOP;
        motor_stop();
        return;
      }

      if ( r_dist > 35 )
      {
        l_motor_PWM = r_motor_PWM = motor_PWM;
        motor_forward( motor_PWM );
        movement_state = MVSTATE_GO_FWD;
      }
      break;
        
    case MVSTATE_KEEP_RIGHT: //due to l_dist < 35
      if ( f_dist < 35)
      {
        movement_state = MVSTATE_STOP;
        motor_stop();
        return;
      }

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
      lcd.setCursor(0,14); // set the cursor to column 0, line 0
      lcd.print(analogRead(A0)*5*3/1023); // Print < to the LCD.        
      lcd.setCursor(0,1); // set the cursor to column 0, line 1
      lcd.print(" MOTOR DEAD STOP"); // Print < to the LCD.
      motor_stop();
      break;    
      
    case MVSTATE_PAUSE:  
      motor_stop();
      break;    
    }

  
}

void put_motor_pause (void)
{
  prev_movement_state = movement_state;
  movement_state = MVSTATE_PAUSE;
}

void resume_motor (void)
{
  movement_state = prev_movement_state;
}

void motor_trick(unsigned int clap_cmd)
{
  int i;
  switch (clap_cmd)
  {
    case 1:       // looks like drawing a Southern Cross pattern
      song_loop();
      for ( i = 0 ; i < 8 ; i++ )
      {
        motor_reverse (40);
        delay(500);
        motor_turn_left(40);
        delay(280);
        motor_forward (40);
        delay(500);
      }
      break;
    case 2:       // forward Zig-Zag pattern
      for ( i = 0 ; i < 4 ; i++ )
      {
        motor_forward (40);
        delay(300);
        motor_turn_left(40);
        delay(300);
        motor_forward (40);
        delay(300);
        motor_turn_right(40);
        delay(300);
      }
      break;
    case 3:
      motor_PWM += 5;
      break;
    case 4:
      motor_PWM -= 5;
      break;
    
  }
  I2C_RGB_LED(ALL_OFF);
}

