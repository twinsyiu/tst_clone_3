#ifndef MOTOR_CTLR_H
#define MOTOR_CTLR_H

// connect motor controller pins to Arduino digital pins
// motor one
//const byte enA = 6;
//const byte in1 = 8;
const byte in1 = 6;     // Speed
const byte in2 = 7;     // Dir
// motor two
//const byte enB = 3;
const byte in3 = 4;     // Speed 
const byte in4 = 5;     // Dir

void motor_stop( void );
void motor_reverse( unsigned int PWM );
void motor_forward( unsigned int PWM );
void motor_turn_left( unsigned int PWM );
void motor_spin_left( unsigned int PWM );

// TY: I will prefer not to explode the motor_drive() to other module
void motor_drive( int L1, int L2,int enA_L, int R1, int R2, int enB_R );

#endif
