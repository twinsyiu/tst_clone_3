#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include "ClapDetect.h"

#ifdef SERIAL_DBG_ON
#undef SERIAL_DBG_ON
#endif

// define a few state in clap detection task
#define CLAPSTAT_IDLE         0
#define CLAPSTAT_DEBOUNCE     1
#define CLAPSTAT_DETECTED     2        // prev clap detected, clap pin LOW waiting for the next

#define CLAP_DEBOUNCE_MS     30
#define CLAP_CMD_PAUSE_US   550000

const byte interruptPin = 2;

bool clap_detected_f = false;
unsigned long clap_start_us;    // clap detected time
bool clap_valid_f = false;
unsigned int clap_valid_cnt = 0;

/*
 * TaskClapDetect_init() initialise the sound sensor interrupt pin
 * and create the FreeRTOS Clap Detect Task
 */
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
  int curr_val;
  unsigned long last_clap_debounced_us;
  unsigned long silent_ts;

  clap_detected_f = false;

  for (;;)
  {
    // Serial.println(" - TaskClapDetect");
    switch (clap_state)
    {
      case CLAPSTAT_IDLE:
        if (clap_detected_f)
        { // when something like a clap is detected
          clap_detected_f = false;            // clear the clap detected flag
          clap_state = CLAPSTAT_DEBOUNCE;     // change to debounce state
          last_clap_start_us = clap_start_us; // store the clap start time stamp captured from ISR log_chg()
          debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;     // working out in how many time the debounce has to be execute
        }
        vTaskDelay( 15 / portTICK_PERIOD_MS); // Task delay 15ms without affecting other system operations
        break;
      case CLAPSTAT_DEBOUNCE:
        // CLAP debounce state; exit this state when clap interrupt pin is LOW for at least 45ms

        // during the debounce state, ignore any clap_detected_f HIGH, clear it if found
        curr_val = digitalRead(interruptPin);
        if (curr_val == HIGH)
        { // the interrupt pin is still high, so reset the debounce count
          // Serial.println("DEBOUNCE curr_val == HI");
          debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;
          clap_detected_f = false;        // clear the flag, no matter any noise
        }
        else
        { // the interrupt pin is LOW
          clap_detected_f = false;        // clear the flag, no matter any noise
          if (debounce_cnt--)
          { // decrement the debounce_cnt if it is not 0
            // do nothing, as the interruptPin is LO, but less than the DEBOUNCE period
            // delay (15ms) in between reads to improve stability
            vTaskDelay( 15 / portTICK_PERIOD_MS);
          }
          else
          { // the debounce_cnt is 0, ie the count down for debounce is completed
            clap_cnt++;     // increment the valid clap counter by 1
            clap_state = CLAPSTAT_DETECTED;
            last_clap_debounced_us = micros();
          }
        }
        break;
      case CLAPSTAT_DETECTED:
        // similar to IDLE, it will detect the next clap
        if (clap_detected_f)
        { // another clap is just detected; go back to debounce state
          // Serial.println("DETECTED, another one just detected");
          clap_detected_f = false;
          clap_state = CLAPSTAT_DEBOUNCE;
          last_clap_start_us = clap_start_us;
          debounce_cnt = CLAP_DEBOUNCE_MS / portTICK_PERIOD_MS;
          break;
        }

        // check how long it has been silent, ie no more clap coming in
        silent_ts = micros() - last_clap_debounced_us;

        // if no more clap comes in, then a clap command is received
        if (silent_ts > (CLAP_CMD_PAUSE_US))
        {
          clap_valid_f = true;        // this flag is like a semaphore, user use the clap_valid_cnt should clear this flag
          clap_valid_cnt = clap_cnt;

          clap_cnt = 0;
          clap_state = CLAPSTAT_IDLE; // reset the state machine to IDLE state
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

/*
 * log_chg() is an ISR when the clap interrupt pin is RISING
 */
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
unsigned int clap_cmd = 0;
bool clap_cmd_rdy_f = false;
bool clap_cmd_pending_f = false;

/*
 * clap_cmd_hdlr() will handle clap commands when received and confirmatin clap received as well
 * it will pause the motor once a valid clap command is received
 */
void clap_cmd_hdlr( void )
{

  unsigned long current_time;

  current_time = millis();    // the millis() will role-over in 63 days (TODO if battery can last that long
  if ( current_time > next_clap_cmd_time_ms )
  {
//    next_clap_cmd_time_ms = current_time + clap_cmd_period_ms;  // clap command will be addressed every 50ms
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
          put_motor_pause();        // pause the motor when a clap command is received
          next_clap_cmd_time_ms = current_time + 250;   // expecting the confirmation clap arrived in 250ms
          break;
        }
        next_clap_cmd_time_ms = current_time + 50;
        break;
      case CMDRSTAT_CNFRM:
        if (clap_valid_f)
        {
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
          else
          {
            clap_valid_cnt = 0;

            // confirmation is invalid, clear the RGB to indicate the clap command is dump
            I2C_RGB_LED(ALL_OFF);
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
          // no valid confirmation clap, so check if the clap command is expired (4 seconds)
          diff_ms = millis() - clap_cmd_ms_ts;
          if ( diff_ms > CMD_CNFRM_TIMEOUT_MS )
          {
            resume_motor();
            I2C_RGB_LED(ALL_OFF);
            cmdr_state = CMDRSTAT_STDBY;
            clap_valid_cnt = 0;
            clap_cmd_rdy_f = false;
          }
          next_clap_cmd_time_ms = current_time + 50;
        }
        break;
       default:
        display_dist("@@@@@");
        break;
    }
  }
}

