/*  Task  Wheel Encoder
 *  It counts the slot in open and close slot as 1 count.
 *  It returns the slot count of Left and Right wheel as second. count_L & count_R
 *  It returns the total count of both wheel from start up.
 *  it calculates the total count considering Motor REV count. tot_count_L & tot_count_R 
 */
void encoder_read(void *p)
{
  int count_L=0;
  int slot_L=0;
 
  int count_R=0;
  int slot_R=0;
 
 
 
//  Serial.print("Motor Start at speed L & R "); Serial.print(set_speed_L);Serial.print("");Serial.println(set_speed_R);
 
  while (1)
  {
 
    if (millis()-startMillis_enco < decon_time) // counter within 5 sec
    {
     if (digitalRead(enco_pin_L)& slot_L==0)
     {
      
 
       count_L++;
       slot_L=1;
  //     Serial.println(count_L);
     }
     if (digitalRead(enco_pin_L)==0)
     {
  //     Serial.println(speed_slot=millis()-slotMillis);
       slot_L=0;
     }
     if (digitalRead(enco_pin_R)& slot_R==0)
     {
 
       count_R++;
       slot_R=1;
     }
     if (digitalRead(enco_pin_R)==0)
     {
  //     Serial.println(speed_slot=millis()-slotMillis);
       slot_R=0;
     }  
   }
   else // after 1 sec
   {
//     Serial.print("speed L & R ="); Serial.print(set_speed_L);Serial.print(" "); Serial.println(set_speed_R);
//      Serial.print("  Slot Count L & R = ");Serial.print(count_L);Serial.print(" ");Serial.println(count_R);
//      Serial.print("  Total Slot Count L & R = ");Serial.print(tot_count_L);Serial.print(" ");Serial.println(tot_count_R);
//  motor(speed_R,speed_L,0);
    if (car_dir_flag==0) //FWD accumate slot count
    {
    tot_count_L = tot_count_L + count_L;
    tot_count_R = tot_count_R + count_R;
    }
    startMillis_enco=millis();
    slot_count_L = count_L;
    slot_count_R = count_R;
//    xSemaphoreGive(speed_adj_signal);
    if (drive_mode==1)
    {
         travel_control(); 
    }
    else
    {
      motor(0,0,0);
    }

    count_L = count_R = 0;
   }
//   vTaskDelay(1); //vTaskDelay used but cause timing issue.
  }
}

