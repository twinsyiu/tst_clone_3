#ifndef MOTOR_CTLR_H
#define MOTOR_CTLR_H

#define MOTOR_L 1
#define MOTOR_R 0
#define DIR_FWD 1
#define DIR_REV 0

void motor_stop( void );
void motor_reverse( unsigned int PWM );
void motor_forward( unsigned int PWM );
void motor_left_fwd( unsigned int PWM );
void motor_left_rev( unsigned int PWM );
void motor_right_fwd( unsigned int PWM );
void motor_right_rev( unsigned int PWM );
void motor_turn_left( unsigned int PWM );
void motor_turn_right( unsigned int PWM );

void motor_drive_sp( unsigned int L_R, unsigned int Fwd_Rev, unsigned int PWM_Spd);

#endif
