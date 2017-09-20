#include "motor_ctlr.h"


void motor_drive( int L1, int L2,int enA_L, int R1, int R2, int enB_R)
{
  Serial.print("Motor Drive = ");
  Serial.print(L1);Serial.print(" ");Serial.print(L2);Serial.print(" ");Serial.print(enA_L);Serial.print(" ");
  Serial.print(R1);Serial.print(" ");Serial.print(R2);Serial.print(" ");Serial.print(enB_R);
  Serial.println("");
  
  // this function will run the motors in both directions at a fixed speed
  // turn on motor A
  //digitalWrite(in1, L1);
  analogWrite(in1, L1);
  digitalWrite(in2, L2);    //  Dir
  // set speed to enA_L out of possible range 0~255
  //analogWrite(enA, enA_L);
  // turn on motor B
  digitalWrite(in3, R1);    // Dir
  //digitalWrite(in4, R2);
  analogWrite(in4, R2);
  // set speed to enB_R out of possible range 0~255
  //analogWrite(enB, enB_R); 
}

void motor_forward( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_forward analog_drive = ");Serial.println(analog_drive);
  motor_drive(0,1,analog_drive,0,1,analog_drive);
}

void motor_reverse( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_reverse analog_drive = ");Serial.println(analog_drive);
  motor_drive(1,0,analog_drive,1,0,analog_drive);
}

void motor_stop( void )
{
  Serial.println("motor_stop IMMEDIATELY !!!");
  motor_drive(0,0,0,0,0,0);
}

void motor_turn_left( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_turn_left analog_drive = ");Serial.println(analog_drive);
  
  // Right motor move forward, Left motor stop
  motor_drive(0,0,0,0,1,analog_drive);
}

void motor_spin_left( unsigned int PWM )
{
  unsigned int analog_drive;
  analog_drive = constrain(PWM, 0, 100) * 255 / 100;
  Serial.print("motor_spin_left analog_drive = ");Serial.println(analog_drive);
  
  // Right motor move forward, Left motor reverse
  motor_drive(1,0,analog_drive,0,1,analog_drive);
}

