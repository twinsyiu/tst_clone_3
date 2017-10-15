#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include "ClapDetect.h"

#ifdef SERIAL_DBG_ON    
#undef SERIAL_DBG_ON    
#endif

#define CLAPSTAT_IDLE         0
#define CLAPSTAT_DEBOUNCE     1
#define CLAPSTAT_DETECTED     2        // prev clap detected, clap pin LOW waiting for the next 

void TaskClapDetect_init( void )
{
  // using RISING edge on sound level sensor
  attachInterrupt(digitalPinToInterrupt(interruptPin), log_chg, RISING);

  // Serial.println("TaskClapDetect: b4 create"); 
  xTaskCreate(
    TaskClapDetect 
    ,  (const portCHAR *) "TaskClapDetect" 
    ,  128  // Stack size
    ,  NULL
    ,  1
    ,  NULL );
  Serial.println("TaskClapDetect: create OK"); 
}

void TaskClapDetect( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  unsigned int clap_state = CLAPSTAT_IDLE;    // waiting a clap
  unsigned int clap_cnt = 0;
  unsigned int debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;
  unsigned long last_clap_start_us;
  unsigned long marker_1_us;
  unsigned long marker_2_us;
  int curr_val;
  unsigned long last_clap_debounced_us;
  unsigned long silent_ts;
  
  clap_detected_f = false;
  Serial.println("TaskClapDetect: BEGIN"); 

  for (;;)
  {
    // Serial.print(micros());
    // Serial.println(" - TaskClapDetect");
    switch (clap_state)
    {
      case CLAPSTAT_IDLE:
        // Serial.println("IDLE "); 
        if (clap_detected_f)
        {
          clap_detected_f = false;
          clap_state = CLAPSTAT_DEBOUNCE;
          last_clap_start_us = clap_start_us;
          debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;
          marker_1_us = micros();
          // Serial.println("IDLE : clap_detected_f TRUE"); 
          vTaskDelay( 15 / portTICK_PERIOD_MS);
        }
        break;
      case CLAPSTAT_DEBOUNCE:
        // Serial.println("CLAPSTAT_DEBOUNCE "); 
        marker_2_us = micros();     // want to know how long does it takes from IDLE to DEBOUNCE
        // exit this state when clap pin is LOW for at least 45ms
        // during the debounce state, ignor any clap_detected_f HIGH, clear it if necessary
        curr_val = digitalRead(interruptPin);
        if (curr_val == HIGH)
        {
          // Serial.println("DEBOUNCE curr_val == HI");
          debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;
          clap_detected_f = false;        // clear the flag, no matter any noise
        }
        else
        {
          clap_detected_f = false;        // clear the flag, no matter any noise
          if (debounce_cnt--)
          {
            // do nothing, as the interruptPin is LO, but less than the DEBOUNCE period
#ifdef SERIAL_DBG_ON    
            Serial.println("DEBOUNCE curr_val == LO");
            Serial.print("last_clap_start_us: "); 
            Serial.print(last_clap_start_us); 
            Serial.print("  marker_1_us: "); 
            Serial.print(marker_1_us); 
            Serial.print("  marker_2_us: "); 
            Serial.print(marker_2_us); 
            Serial.print("  debounce_cnt: "); 
            Serial.print(debounce_cnt); 
            Serial.print("  curr_ts: "); 
            Serial.println(micros()); 
#endif
            //vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
            vTaskDelay( 15 / portTICK_PERIOD_MS);
          }
          else
          { // the debounce_cnt is 0, ie the count down to debounce is completed
            clap_cnt++;
            clap_state = CLAPSTAT_DETECTED;
            last_clap_debounced_us = micros();
          }
        }
        break;
      case CLAPSTAT_DETECTED:
        // Serial.println("CLAPSTAT_DETECTED");
        // similar to IDLE, it will detect the next clap
        if (clap_detected_f)
        {
          // Serial.println("DETECTED, another one just detected");
          clap_detected_f = false;
          clap_state = CLAPSTAT_DEBOUNCE;
          last_clap_start_us = clap_start_us;
          debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;
          // marker_1_us = micros();
          break;
        }

        silent_ts = micros() - last_clap_debounced_us;
#ifdef SERIAL_DBG_ON    
        if (silent_ts > 100000)
        {
          Serial.print("DETECTED silent_ts:");
          Serial.println(silent_ts);
        }
#endif
        if (silent_ts > (CLAP_CMD_PAUSE_US))
        {
          clap_valid_f = true;        // this flag is like a semaphore, user took the valid_cnt should clear this flag
          clap_valid_cnt = clap_cnt;

          clap_cnt = 0;
          clap_state = CLAPSTAT_IDLE;
#ifdef SERIAL_DBG_ON    
          Serial.print("  clap_valid_cnt: "); 
          Serial.println(clap_valid_cnt); 
#endif
        }
        //vTaskDelay(10);  // nof tick delay (15ms)
        vTaskDelay( 15 / portTICK_PERIOD_MS);
        break;
      default:
        // Serial.println("CLAPSTAT_default");
        break;
    }
    vTaskDelay( 30 / portTICK_PERIOD_MS);
  }
}

void log_chg()
{
  // set clap_flag if it is not set, it will be cleared in the clap task
  clap_detected_f = true;
  // set clap detected time
  clap_start_us = micros();
}

