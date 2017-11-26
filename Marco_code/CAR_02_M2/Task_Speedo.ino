void Task_Speedo( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  int ulta_expire_time_ms;    // when no ultrasound echo within this time, it will expire
  unsigned long start_trig_ts_ms, start_measure_ts_ms;
  String stringOne;

//Serial.println("Tk_Spdo 1");

  for (;;) // A Task shall never return or exit.
  {
    start_measure_ts_ms = millis();
    for ( int ulta_id = 0; ulta_id <= R_ULTA_ID; ulta_id++ )
    {
      ultr_dist[ulta_id] = 0.0;
      start_trig_ts_ms = millis();
      ultrasound_trigger( ulta_id );
      do {
        vTaskDelay(30 / portTICK_PERIOD_MS);  // delay 15ms
        if ( echo_hi_ts_us && echo_lo_ts_us  && (echo_lo_ts_us > echo_hi_ts_us ) && !digitalRead(com_echoPin) )
        {
          // this is a valid return from the ultrasound sensor
          //Serial.print("front dist: "); Serial.println( constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399) );
          // max 399cm ie 
          ultr_dist[ulta_id] = constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399);
          break;  // break the do-while loop
        }
      }
      while ( (ultr_dist[ulta_id] <= 0) && (( millis() - start_trig_ts_ms ) < 45 ) );    // 
    }
    
    f_dist = ultr_dist[F_ULTA_ID];
    l_dist = ultr_dist[L_ULTA_ID];
    r_dist = ultr_dist[R_ULTA_ID];

/*
*/
  vTaskDelay(200 / portTICK_PERIOD_MS);  // nof tick delay (45ms)
  vTaskDelay(200 / portTICK_PERIOD_MS);  // nof tick delay (45ms)

  // check if it the wheel stucked
  // =============================================

  get_encoder_1s_count(&encoder); // getting struct encoder data

  if ( (encoder.L_E_count_1s < 5 or encoder.R_E_count_1s < 5) && movement_state != MVSTATE_DEAD_STOP ) // only if not Low voltage DEAD_STOP
  {
    motor_stuck_turn(); 
  }

  
  // check supply voltage for protection
   if ( analogRead(A0)*5*3/1023 < 9.5 )
   {
    movement_state = MVSTATE_DEAD_STOP;
   }
   else if (movement_state == MVSTATE_DEAD_STOP) //switch back from MVSTATE_DEAD_STOP
   {
    movement_state = MVSTATE_GO_FWD;
   }

 }  
}

