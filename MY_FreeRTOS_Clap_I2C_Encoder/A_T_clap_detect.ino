void clap_detect(void *p)
{
  /*    < Task for clap detect and counting >
   * The task results a command mode from the clap_cnt_command = 1 to 4
   */
  long startMillis_TB; //timebase;
//  long startMillis_sys = millis();
  int clap_cnt_confirming =0;
  int clap_cnt_temp =0;

  while (1)
  {
//    Serial.println("Task CLAP");
    /*
     * The first part of code is for power start up switching between Auto mode and 
     * command mode. The second part serve the command mode to count the clap to command
     */
    if (millis()- startMillis_sys < 10000)
    {
      Serial.print("going to sys_mode_control, sys_mode = ");Serial.println(sys_mode);
      sys_mode_control();
      clap_cnt = clap_cnt_temp =0; //reset to prevent unwanted clap_cnt as enter command mode
    }
    else if (sys_mode ==0) //auto mode
    {
      drive_mode=1;
      flash_RGB(8,5); //double green LED
    }
    else
    {
      /* the second part
     * Two states for clap counting/detecting
     * clap_cnt_confirming mode is for user to verify the clap command with clap count clap_cnt
     * if not clap_cnt_confirming mode, clap counter of user will be captured within 1 sec time base & 
     * pass it on to clap_cnt_confirming mode.
     */
 
      if (!clap_cnt_confirming)
      {
        if (clap_cnt==1) //The first clap detected & mark start time  of the timebase 1 sec count down
        {
         startMillis_TB=startMillis_ISR;  
        }
        if (clap_cnt >0)
        {  
  //      Serial.print("  clap_cnt >0 ");Serial.println(millis()-startMillis_TB);
          if ( millis()-startMillis_TB > 1000) // timebase 1 sec countdown
          {
            if (clap_cnt < 4)
            {
              clap_cnt_temp = clap_cnt;
              clap_cnt_confirming = 1; // flag to switch to clap_cnt_confirming state
              I2C_RGB_LED(clap_cnt_temp);   // LED notify the user of the clap count / command
            }
            else 
            {
              Serial.println("invalid clap");
              clap_cnt = clap_cnt_temp = clap_cnt_command = 0; //Reset all  clap count for invalid clap
              flash_RGB(5,8); // flash all LED showing invalid command clap
              vTaskDelay(100); //Delay to reset clap_cnt
            }
            vTaskDelay(10); // Delay to uncount excess clap rebounce
            clap_cnt = 0;
            Serial.print("clap_cnt_temp ");Serial.println(clap_cnt_temp);
            startMillis_TB = millis(); //reset startMillis_TB for clap_cnt_confirming  2 sec count down
          }
        }    
      }
      if (clap_cnt_confirming)
      {
        if ( millis()-startMillis_TB < 2000) // within 2 sec waiting period for user to verify the clap command
        {
          if (clap_cnt==1) // clap detected & showing with flashing LED
          {
            clap_cnt_command = clap_cnt_temp;
            flash_RGB(clap_cnt_command,4);
            motor_drive(clap_cnt_command);             
            clap_cnt_confirming = 0; // switch clap_cnt_confirming state of WITHIN 2 sec waiting count down
            clap_cnt = 0;
            
            Serial.print("clap confirmed & command = "); Serial.println(clap_cnt_command); 
          }
        }
        else
        {
          Serial.println("nothing confirmed $ command ");
          clap_cnt_confirming = 0; // switch clap_cnt_confirming state AFTER 2 sec waiting count down 
          I2C_RGB_LED(0);
        }
      }
      vTaskDelay(1);
    }
  }
}

