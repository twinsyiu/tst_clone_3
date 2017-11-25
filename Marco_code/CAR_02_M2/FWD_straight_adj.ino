//  unsigned int motor_PWM, movement_state, l_motor_PWM, r_motor_PWM;
  encoder_Struct encoder;
  int l_faster, prev_l_faster;


void FWD_straight_adj(void)
{
          // try to go straight, adjust pwm if necessary
        // =============================================
          l_faster = get_encoder_1s_count( &encoder);
        Serial.print("L_E= ");Serial.print(encoder.L_E_count_1s);
        Serial.print(" R_E= ");Serial.print(encoder.R_E_count_1s);
        Serial.print("     l_faster= ");Serial.println(l_faster);        
         
        if ( l_faster > 0 )
        { 
          // l_faster > 0 : left wheel is faster
          // compensate by slow down the left wheel
          if ( prev_l_faster > 0 )
          {
            l_motor_PWM = motor_PWM - 1; //MY
            r_motor_PWM = motor_PWM + 1; //MY
          
//          Serial.print("(-) ");
        } 
        else if ( l_faster < 0 )
        { 
          //   l_faster < 0 : left wheel is slower
          // speed up the left wheel
          if ( prev_l_faster < 0 )
          {
            l_motor_PWM = motor_PWM + 1; //MY
            r_motor_PWM = motor_PWM - 1; //MY
         }            

          }
//          Serial.print("(+) ");
        } // else equal, do nothing
        else
        { // l_faster is 0 ; ie the encoder counters are equal, the best time to reset the counters
//          Serial.print("( ) ");
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
        } // else equal, do nothing

        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
       
        prev_l_faster = l_faster;

        delay(200);

}

