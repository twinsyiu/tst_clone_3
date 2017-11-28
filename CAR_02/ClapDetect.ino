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

/*
 * TaskClapDetect() is a FreeRTOS Task to handle clapping commands
 */
void TaskClapDetect( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  unsigned int clap_state = CLAPSTAT_IDLE;    // waiting on a clap command
  unsigned int clap_cnt = 0;
  unsigned int debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;
  unsigned long last_clap_start_us;
  unsigned long marker_1_us;
  unsigned long marker_2_us;
  int curr_val;
  unsigned long last_clap_debounced_us;
  unsigned long silent_ts;
  
  clap_detected_f = false;

  for (;;)
  {
    // Serial.print(micros());
    // Serial.println(" - TaskClapDetect");
    switch (clap_state)
    {
      case CLAPSTAT_IDLE:
        if (clap_detected_f)
        { // when something like a clap is detected
          clap_detected_f = false;            // clear the clap detected flag
          clap_state = CLAPSTAT_DEBOUNCE;     // change to debounce state 
          last_clap_start_us = clap_start_us; // capture the clap start time stamp
          debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;     // working out in how many time the debounce has to be execute
          marker_1_us = micros();             // marker_1 is the timestamp of current micros()
        }
        vTaskDelay( 15 / portTICK_PERIOD_MS);
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
            //delay (15ms) in between reads to improve stability
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
        
        // if no more clap comes in, then a clap command is received
        if (silent_ts > (CLAP_CMD_PAUSE_US))
        {
          clap_valid_f = true;        // this flag is like a semaphore, user took the valid_cnt should clear this flag
          clap_valid_cnt = clap_cnt;

          clap_cnt = 0;
          clap_state = CLAPSTAT_IDLE;
        }
        
        vTaskDelay( 15 / portTICK_PERIOD_MS); // delay (15ms)
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

#define CMDRSTAT_STDBY     0
#define CMDRSTAT_CNFRM     1
#define CMD_CNFRM_TIMEOUT_MS     4000


const unsigned int clap_cmd_period_ms = 50;
unsigned int cmdr_state = CMDRSTAT_STDBY;    // waiting a clap
unsigned long next_clap_cmd_time_ms;
unsigned long clap_cmd_ms_ts, diff_ms;
unsigned int clap_cnt_msg;
//byte colour_arry[5] = {COLOUR_NIL, COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE, COLOUR_WHITE};
unsigned int clap_cmd = 0;
bool clap_cmd_rdy_f = false;
bool clap_cmd_pending_f = false;

void clap_cmd_hdlr( void )
{
  
  unsigned long current_time;

  current_time = millis();                // the millis() will role-over in 63 days (TODO if battery can last that long
  if ( current_time > next_clap_cmd_time_ms )
  {
    next_clap_cmd_time_ms = current_time + clap_cmd_period_ms;
    switch (cmdr_state)
    {
       case CMDRSTAT_STDBY:
        if (clap_valid_f)
        {
          clap_valid_f = false;     // clear the valid flag
          clap_cmd = clap_valid_cnt;
          // display RGB
          I2C_RGB_LED(clap_valid_cnt);

          cmdr_state = CMDRSTAT_CNFRM;
          clap_cmd_ms_ts = millis();
          put_motor_pause();
        }
        next_clap_cmd_time_ms = current_time + 50;
        break;
      case CMDRSTAT_CNFRM:
        if (clap_valid_f)
        {
          resume_motor();
          clap_valid_f = false;     // clear the valid flag
          if (clap_valid_cnt == 1)  // a confirmation clap is picked-up
          {
            clap_valid_cnt = 0;
            
            // clap_cmd is valid, display RGB to confirm the Confirmation clap is received
            I2C_RGB_LED(clap_cmd + 4);

            clap_cmd_rdy_f = true;
            // take action at here <<<<<<<<<<<<<<<<<<<<<<<< PLAY SOME TRICKS
            motor_trick(clap_cmd);
            resume_motor();
            cmdr_state = CMDRSTAT_STDBY;
            next_clap_cmd_time_ms = current_time + 50;
            break;
          }
          // else stay inside the CMDRSTAT_CNFRM state until it timeout or a valid confirmation is received
          next_clap_cmd_time_ms = current_time + 250;
        }
        else
        {
          resume_motor();
          // no valid confirmation clap, so check if the clap command is expired
          diff_ms = millis() - clap_cmd_ms_ts;
          if ( diff_ms > CMD_CNFRM_TIMEOUT_MS )
          {
            I2C_RGB_LED(ALL_OFF);
            cmdr_state = CMDRSTAT_STDBY;
            clap_valid_cnt = 0;
            clap_cmd_rdy_f = false;
          }
          next_clap_cmd_time_ms = current_time + 250;
        }
        break;
       default:
        display_dist("@@@@@");
        break;    
    }
  }
}

