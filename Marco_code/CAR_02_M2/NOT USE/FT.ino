////  unsigned int motor_PWM, movement_state, l_motor_PWM, r_motor_PWM;
//  encoder_Struct encoder;
//  int l_faster, prev_l_faster;
//
//
//void FWD_straight_adj(void)
//{
//          // try to go straight, adjust pwm if necessary
//        // =============================================
//        l_faster = get_encoder( &encoder );
      
////        Serial.print("= left enc cnt : "); Serial.print(encoder.l_encoder_count);
////        Serial.print(" right enc cnt : "); Serial.print(encoder.r_encoder_count);
////        Serial.print(" :: ");
//         
//        if ( l_faster > 0 )
//        { // l_faster > 0 : left wheel is faster
//          // compensate by slow down the left wheel
//          if ( prev_l_faster > 0 )
//          {
//            if ( l_motor_PWM > 12 )
//              l_motor_PWM -= 2;     // double the decreament if prev sample indicating the left was faster already
//            else
//              if ( (r_motor_PWM >= motor_PWM) && ((r_motor_PWM - motor_PWM) < 10) )
//                r_motor_PWM += 2;
//          }
//          else
//          {
//            if ( l_motor_PWM > 11 )
//              l_motor_PWM -= 1;
//            else
//              if ( (r_motor_PWM >= motor_PWM) && ((r_motor_PWM - motor_PWM) < 10) )
//                r_motor_PWM += 1;
//          }
//          
////          Serial.print("(-) ");
//        } 
//        else if ( l_faster < 0 )
//        { //   l_faster < 0 : left wheel is slower
//          // speed up the left wheel
//          if ( prev_l_faster < 0 )
//          {
//            l_motor_PWM += 2;     // double the increament if prev sample indicating the left was slower 
//            if ( (l_motor_PWM - motor_PWM) > 10 )
//            {
//              l_motor_PWM -= 2;
//              r_motor_PWM -= 2;
//            }            
//          }
//          else
//          {
//            l_motor_PWM += 1;
//            if ( (l_motor_PWM - motor_PWM) > 10 )
//            {
//              l_motor_PWM -= 1;
//              r_motor_PWM -= 1;
//            }            
//
//          }
////          Serial.print("(+) ");
//        } // else equal, do nothing
//        else
//        { // l_faster is 0 ; ie the encoder counters are equal, the best time to reset the counters
////          Serial.print("( ) ");
//          reset_encoder_count();
//          prev_l_faster = 0;
//          l_motor_PWM = r_motor_PWM = motor_PWM;
//        } // else equal, do nothing
//        PWM_adjust (&l_motor_PWM, &r_motor_PWM, 5, motor_PWM);
//
//        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
//        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
//
//        
//        prev_l_faster = l_faster;
//        //Serial.print(l_motor_PWM); Serial.print(" | "); Serial.println(r_motor_PWM);
////        Serial.print("    left enc cnt : "); Serial.print(encoder.l_encoder_count);
////        Serial.print("    righ enc cnt : "); Serial.println(encoder.r_encoder_count);
//
//        delay(200);
//
//}

