#include "motor_ctlr.h"


void motor_drive( int Ldir,int Lspd, int Rdir, int Rspd)
{
  Serial.print("Motor Drive = ");
  Serial.print(Ldir);Serial.print(" ");Serial.print(Lspd);Serial.print(" ");
  Serial.print(Rdir);Serial.print(" ");Serial.print(Rspd);
  
  // this function will run the motors in both directions at a fixed speed
  // turn on motor A
  digitalWrite(in1dir, Ldir);    //  Dir
  analogWrite(in2spd, Lspd);     // speed
  // set speed to enA_L out of possible range 0~255
  // turn on motor B
  digitalWrite(in3dir, Rdir);    // Dir
  analogWrite(in4spd, Rspd);     // speed
  // set speed to enB_R out of possible range 0~255
}

void motor_forward( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_forward analog_drive = ");Serial.println(analog_drive);
//  motor_drive(0,analog_drive,0,analog_drive);   // only the Or/Bw motor roll to 3rd wheel
  motor_drive(0,analog_drive,0,analog_drive);
}

void motor_reverse( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_reverse analog_drive = ");Serial.println(analog_drive);
//  motor_drive(1,analog_drive,1,analog_drive);   // only the R/B motor roll away from 3rd wheel
  motor_drive(1,analog_drive,1,analog_drive);
}

void motor_stop( void )
{
  Serial.println("motor_stop IMMEDIATELY !!!");
  motor_drive(0,0,0,0);
}

void motor_turn_left( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_turn_left analog_drive = ");Serial.println(analog_drive);
  
  // Right motor move forward, Left motor stop
  motor_drive(0,0,1,analog_drive);
}

void motor_spin_left( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_spin_left analog_drive = ");Serial.println(analog_drive);
  
  // Right motor move forward, Left motor reverse
  motor_drive(0,analog_drive,1,analog_drive);
}

