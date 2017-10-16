void UltraSound_detect(void *p)
{
  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
  unsigned long crash_check_start = millis();
  unsigned int ping_sample_cur =1000;
  unsigned int ping_sample_2 = 1000;
  unsigned int ping_sample_3 =1000;
  unsigned int ping_average = 50;
  while(1)
  {  
//    Serial.println("Task Ultrasound");
    unsigned int uS = sonar.ping();// Send ping, get ping time in microseconds (uS).
//    Serial.print("in ping Task ping -cm = "); Serial.println(uS/US_ROUNDTRIP_CM);
    ping_sample_3 = ping_sample_2;
    ping_sample_2 =ping_sample_cur;
    ping_sample_cur = uS/US_ROUNDTRIP_CM; //return distance in cm
    ping_average = (ping_sample_3+ping_sample_2+ping_sample_cur)/3;
//    Serial.print("ping:");
//    Serial.print(ping_sample_cur); Serial.print("   ");Serial.println(ping_average);
//    if (ping_sample_cur >0 && ping_sample_cur < 20 && ping_average < 27)
//    {
//      flash_RGB(3,2);
//      motor_drive(0); //stop
//      delay(2);
//      motor_drive(2); //REV
//      delay(500);
//      motor_drive(0); 
//    }
    crash_check_start = millis();
    vTaskDelay(30); 
  }             
}

