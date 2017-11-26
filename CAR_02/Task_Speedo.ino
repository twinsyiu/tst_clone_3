float ultr_dist[3];

/*
 * Task_Speedo() is FreeRTOS Task which manage to get the nearly real-time distance of objects from three ultrasound sensors.
 * only one sensor will be triggered at one time,
 * the echo received will be comming back from the triggered sensor
 */
void Task_Speedo( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  int ulta_expire_time_ms;    // when no ultrasound echo within this time, it will expire
  unsigned long start_trig_ts_ms;

  for (;;) // A Task shall never return or exit.
  {
    for ( int ulta_id = 0; ulta_id <= R_ULTA_ID; ulta_id++ )
    {
      ultr_dist[ulta_id] = 0.0;
      start_trig_ts_ms = millis();
      ultrasound_trigger( ulta_id );
      do {
        vTaskDelay(30 / portTICK_PERIOD_MS);  // delay 15ms
        if ( echo_hi_ts_us && echo_lo_ts_us && !digitalRead(com_echoPin) )
        {
          // this is a valid return from the ultrasound sensor
          // echo_hi_ts_us and echo_lo_ts_us are non-zero, and com_echoPin is LOW
          // Serial.print("front dist: "); Serial.println( constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399) );
          // ie. max 399cm

          // since SOUND_SPEED_IN_AIR = 0.0343 cm/us
          // distance_from_object = time_in_micro_second * SOUND_SPEED_IN_AIR / 2

          ultr_dist[ulta_id] = constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399);
          break;  // break the do-while loop
        } 
        else if ( !echo_lo_ts_us )
        {
          // Serial.println("echo_lo_ts_us is 0, out of range, MAX 377cm"); 
          ultr_dist[ulta_id] =  377;
        } 
        else if ( echo_hi_ts_us == echo_lo_ts_us )
        {
          // Serial.println("ERROR dist: echo_hi_ts_us = echo_lo_ts_us"); 
          ultr_dist[ulta_id]  = 366;
        }
        else
        {
          ultr_dist[ulta_id]  = 388;
        }
      }
      while ( (ultr_dist[ulta_id] <= 0) && (( millis() - start_trig_ts_ms ) < 45 ) );    // 
      Serial.print(ulta_id);Serial.print(":");Serial.print(start_trig_ts_ms);Serial.print(" --ult-- ");Serial.println(millis());
  
    }
    
    f_dist = ultr_dist[F_ULTA_ID];
    l_dist = ultr_dist[L_ULTA_ID];
    r_dist = ultr_dist[R_ULTA_ID];

    vTaskDelay(100 / portTICK_PERIOD_MS);  // delay (100ms)
   
  }
}

