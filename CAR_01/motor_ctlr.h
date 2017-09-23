#ifndef MOTOR_CTLR_H
#define MOTOR_CTLR_H

void motor_stop( void );
void motor_reverse( unsigned int PWM );
void motor_forward( unsigned int PWM );
void motor_left_fwd( unsigned int PWM );
void motor_left_rev( unsigned int PWM );
void motor_right_fwd( unsigned int PWM );
void motor_right_rev( unsigned int PWM );
void motor_turn_left( unsigned int PWM );
void motor_turn_right( unsigned int PWM );


#endif
