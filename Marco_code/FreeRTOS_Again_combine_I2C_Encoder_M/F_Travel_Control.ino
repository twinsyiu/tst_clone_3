/*  Travel Control Task
 *  It engaged the slot count slot_count_L & slot_count_R, and 
 *  both wheels rotation slot, tot_count_L & to_count_R
 *  It adjust motor speed according with different of varable above.
 */
void travel_control(  )
{
  int speed_inc =2*per_cent; //speed adj incremental
  int tot_count_diff = 5;
//  int speed_L = set_speed_L; //Mark the set speed before speed correction
//  int speed_R = set_speed_R;
 
  
     if (car_dir_flag==0) //FWD
     {
//      Serial.print(slot_count_L);Serial.print(" L & R ");Serial.println(slot_count_R);
      /*  Travel in Stright Line
       *  Control by Encoders of motors and counts to adjust motor speed
       */
      if (slot_count_L > slot_count_R)
      {
        speed_L = speed_L - speed_inc;
        if (tot_count_L - tot_count_R > tot_count_diff)
          {
           speed_R= speed_R + speed_inc; 
          }
      }
      else if (slot_count_R > slot_count_L)
      {
        speed_L = speed_L + speed_inc;
        if (tot_count_R - tot_count_L > tot_count_diff)
          {
           speed_R= speed_R - speed_inc; 
          }
      }   
//      Serial.print(" speed L & R ="); Serial.print(speed_L);Serial.print(" "); Serial.println(speed_R);
//       Serial.print(" total count diff L - R ="); Serial.println(tot_count_L - tot_count_R);
  //     Serial.print(" tot_count_L= ");Serial.println(tot_count_L);
      if (speed_L > 255 or speed_R >255)
      {
        speed_L= set_speed_L; //reset speed as adnormal correction
        speed_R= set_speed_R;
        //RGB_LED(0); //Alarm
      }
      /*
       * Maintain speed as set speed  due to correction of speed
       * because correction may 
       */
      if (speed_L > set_speed_L && speed_R >set_speed_R)
      {
        speed_L= speed_L - speed_inc ;
        speed_R= speed_R -speed_inc;
      }
      else if ( speed_L < set_speed_L && speed_R < set_speed_R)
      {
        speed_L= speed_L + speed_inc;
        speed_R= speed_R + speed_inc;
      }
     }
  
      
      if ( car_dir_flag == 1)
      {
        Serial.println("REV");
        motor_drive(2);   //REV config
        tot_count_L= tot_count_L - slot_count_L;
        tot_count_R= tot_count_R - slot_count_L;
        if ( tot_count_L <= 0 or tot_count_R <=0 )
        {
          car_dir_flag=0; 
        }
      }
      
      if (car_dir_flag==0)
      {
  
        motor_drive(1); //FWD
 
      }
      
  //    vTaskDelay(50); // important to allow the Encoder to read for 1 sec before change speed again
    
  
}


