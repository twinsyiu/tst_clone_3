#include "encoder.h"

/*
 * FWD_straight_adj() will use the encoder readings and adjust the left and right motor PWM accordingly,
 * but both left and right motor PWM can not exceed the range of 16(min_PWM) and motor_PWM+10
 */
void FWD_straight_adj(void)
{
  int l_faster;
  encoder_Struct encoder;

  l_faster = get_encoder( &encoder);
  Serial.print("L_E= ");Serial.print(encoder.L_E_count_1s);
  Serial.print(" R_E= ");Serial.print(encoder.R_E_count_1s);
  Serial.print("     l_faster= ");Serial.println(l_faster);

  if ( l_faster > 0 )
  {
    // l_faster > 0 : left wheel is faster
    // compensate by slow down the left wheel
    l_motor_PWM = motor_PWM - 1; //MY
    r_motor_PWM = motor_PWM + 1; //MY

    // Serial.print("(-) ");
  }
  else if ( l_faster < 0 )
  {
    // l_faster < 0 : left wheel is slower
    // speed up the left wheel
    l_motor_PWM = motor_PWM + 1; //MY
    r_motor_PWM = motor_PWM - 1; //MY

    //  Serial.print("(+) ");
  }
  // else equal, do nothing
  else
  { // l_faster is 0 ; ie the encoder counters are equal, the best time to reset the counters
    // Serial.print("( ) ");
    reset_encoder_count();
    l_motor_PWM = r_motor_PWM = motor_PWM;
  } // else equal, do nothing

  // limit the left and right motor in between minium PWM (16%) and maximum (ie current motor PWM settings + 10%)
  l_motor_PWM = constrain(l_motor_PWM, 16, motor_PWM+10);
  r_motor_PWM = constrain(r_motor_PWM, 16, motor_PWM+10);
  motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
  motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );

}

