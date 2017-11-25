void sys_mode_control()
{
  
  flash_RGB(7,2); // blue blue
//  flash_RGB(4,2); // blue
//  flash_RGB(2,2); // green
//  flash_RGB(5,2); //  green
  if (clap_cnt < 1) //no command
  {
    Serial.print("auto Mode, Clap_cnt = ");Serial.println(clap_cnt);
    sys_mode = 0; //flag auto mode
    travel_mode= 0; //flag NO travel_control    
  }
  else 
  {
   Serial.println("command mode");
   sys_mode = 1;
   travel_mode = 1;
   flash_RGB(10,3);//command mode double white LED
   clap_cnt =  0;
   startMillis_sys = millis()+10000; //terminate the sys_control_mode
  }
}

