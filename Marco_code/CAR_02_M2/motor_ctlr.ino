#include "motor_ctlr.h"

#define DIR_IO_IDX 0
#define PWM_IO_IDX 1

// connect motor controller pins to Arduino digital pins
// motor one
const byte in1dir = 7;     //  Dir grey
const byte in2spd = 6;     //  Speed  white
// motor two
const byte in4spd = 5;     //  Speed blue
const byte in3dir = 4;     // Dir red


unsigned int motor_io[2][2] = {
  {in1dir, in2spd},     // motor_io[0] : motor A Left
  {in3dir, in4spd}      // motor_io[1] : motor B Right
};

void motor_drive_sp( unsigned int L_R, unsigned int Fwd_Rev, unsigned int PWM_Spd)
{
  unsigned int act_PWM;
  unsigned int corrected_Fwd_Rev;

  #ifdef DEBUG
  Serial.print("Motor (L:0/R:1)= ");Serial.print(L_R);
  Serial.print(" Dir (Fwd:0/Rev:1)= ");Serial.print(Fwd_Rev);
  Serial.print(" PWM=");  Serial.println(PWM_Spd);
  if (L_R){Serial.print("L_");} else {Serial.print("R_");}
  if (Fwd_Rev){Serial.print("Fwd");}else{Serial.print("Rev");}
  Serial.print(" Speed=");Serial.println(PWM_Spd);
  #endif

  // either one of the motor wiring (polarity) are swapped; thus introduced the Fwd_Rev correction
  if (L_R)
  {
    corrected_Fwd_Rev = Fwd_Rev;
  }
  else
  {
    corrected_Fwd_Rev = !Fwd_Rev;
  }
  
  // this function will run the motors in both directions at a fixed speed
  digitalWrite(motor_io[L_R][DIR_IO_IDX], corrected_Fwd_Rev);      //  Dir
  if ( corrected_Fwd_Rev )
  {   // 1: REV  -- needs to work out the complement of PWM as the logic within the L298 inversed by current design to save two IO pins
    act_PWM = ( 100 - constrain(PWM_Spd, 0, 100) ) * 255 / 100;
  }
  else
  {   // 0: FWD
    act_PWM = PWM_Spd * 255 / 100;
  }
  analogWrite( motor_io[L_R][PWM_IO_IDX], act_PWM );     // speed
}

// MY motor stuck and turn
void motor_stuck_turn (void)
{
  if ( movement_state == MVSTATE_GO_FWD)
  {
    motor_reverse(motor_PWM * 0.8);
    delay(2000); // allow room to turn
    if ( l_dist > 35 && r_dist > 35)
    {
      motor_turn_left(motor_PWM * 0.6);
      delay(500); // make sure enough turn
        motor_forward(motor_PWM);
    }
    else if ( l_dist > 35)
    {
      motor_turn_left(motor_PWM * 0.6);
      delay(500); // make sure enough turn
        motor_forward(motor_PWM);
    }
    else
    {
      motor_turn_right(motor_PWM * 0.6);
      delay(500); // make sure enough turn
        motor_forward(motor_PWM);
    }
  }
  else
  {
      motor_forward(motor_PWM * 0.8);
    delay(2000); // allow room to turn
    if ( l_dist > 35 && r_dist > 35)
    {
      motor_turn_left(motor_PWM * 0.6);
      delay(500); // make sure enough turn
      movement_state = MVSTATE_GO_FWD;
    }
    else if ( l_dist > 35)
    {
      motor_turn_left(motor_PWM * 0.6);
      delay(500); // make sure enough turn
      movement_state = MVSTATE_GO_FWD;
    }
    else
    {
      motor_turn_right(motor_PWM * 0.6);
      delay(500); // make sure enough turn
      movement_state = MVSTATE_GO_FWD;
    }
  }
}

void motor_forward( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_forward PWM = ");Serial.println(PWM); 
  #endif
  motor_drive_sp( MOTOR_R, DIR_FWD, PWM ); // MY manual adjust speed to bal motor
  motor_drive_sp( MOTOR_L, DIR_FWD, PWM );
}

void motor_stop( void )
{
  #ifdef DEBUG 
  Serial.println("motor_stop IMMEDIATELY !!!");
  #endif
  motor_drive_sp( MOTOR_R, DIR_FWD, 0 );
  motor_drive_sp( MOTOR_L, DIR_FWD, 0 );
}

void motor_reverse( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_reverse PWM = ");Serial.println(PWM);
  #endif
  motor_drive_sp( MOTOR_R, DIR_REV, PWM );
  motor_drive_sp( MOTOR_L, DIR_REV, PWM );
}

// MY Drive change dircetion
void motor_chg_dir( unsigned int l_motor_PWM, unsigned int r_motor_PWM )
{
  motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
  motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
  
}


void motor_turn_left( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_turn_left PWM = ");Serial.println(PWM);
  #endif
  motor_drive_sp( MOTOR_R, DIR_FWD, PWM );
  motor_drive_sp( MOTOR_L, DIR_REV, PWM );
}

void motor_turn_right( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_turn_right PW = ");Serial.println(PWM);
  #endif
  motor_drive_sp( MOTOR_R, DIR_REV, PWM );
  motor_drive_sp( MOTOR_L, DIR_FWD, PWM );
}

void motor_right_fwd( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_right_fwd PWM = ");Serial.println(PWM);
  #endif
  motor_drive_sp( MOTOR_R, DIR_FWD, PWM );
}

void motor_right_rev( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_right_rev PWM = ");Serial.println(PWM);
  #endif
  motor_drive_sp( MOTOR_R, DIR_REV, PWM );
}

void motor_left_fwd( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_left_fwd PWM = ");Serial.println(PWM);
  #endif
  motor_drive_sp( MOTOR_L, DIR_FWD, PWM );
}

void motor_left_rev( unsigned int PWM )
{
  #ifdef DEBUG 
  Serial.print("motor_left_rev PWM = ");Serial.println(PWM);
  #endif
  motor_drive_sp( MOTOR_L, DIR_REV, PWM );
}

