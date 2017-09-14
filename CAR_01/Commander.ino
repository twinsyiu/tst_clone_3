#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include "Commander.h"
#include "ClapDetect.h"
#include "RGBLED_Hdlr.h"

#define CMDRSTAT_STDBY     0
#define CMDRSTAT_CNFRM     1

#define CMD_CNFRM_TIMEOUT_MS     4000

void TaskCommander_init( void )
{
  // using RISING edge on sound level sensor
  // attachInterrupt(digitalPinToInterrupt(interruptPin), log_chg, RISING);
  RGB_LED_set(COLOUR_NIL);
}

void TaskCommander( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  unsigned int cmdr_state = CMDRSTAT_STDBY;    // waiting a clap
  unsigned int clap_cmd = 0;
  unsigned int clap_cmd_ms_ts, diff_ms;
  
  for (;;)
  {
    switch (cmdr_state)
    {
      case CMDRSTAT_STDBY:
        if (clap_valid_f)
        {
          clap_valid_f = false;     // clear the valid flag
          clap_cmd = clap_valid_cnt;
          // display RGB
          RGB_LED_set(colour_arry[constrain(clap_valid_cnt,0,4)]);
          
          cmdr_state = CMDRSTAT_CNFRM;
          clap_cmd_ms_ts = millis();
#ifdef SERIAL_DBG_ON    
          Serial.print("clap_cmd_ms_ts : ");
          Serial.print(clap_cmd_ms_ts);
          Serial.print("  clap_cmd : ");
          Serial.println(clap_cmd);
#endif
        }
        vTaskDelay( 50 / portTICK_PERIOD_MS);  // delay 50ms
        break;
      case CMDRSTAT_CNFRM:
        if (clap_valid_f)
        {
#ifdef SERIAL_DBG_ON    
          Serial.print("clap_valid_f : TRUE ======= clap_valid_cnt : ");
          Serial.println(clap_valid_cnt);
#endif
          clap_valid_f = false;     // clear the valid flag
          if (clap_valid_cnt == 1)
          {
            // clap_cmd is valid, blink to confirm the Confirmation clap is received
            RGB_LED_set(COLOUR_NIL);
            vTaskDelay( 400 / portTICK_PERIOD_MS);  // delay 450ms
            RGB_LED_set(colour_arry[constrain(clap_cmd,0,4)]);
            vTaskDelay( 500 / portTICK_PERIOD_MS);  // delay 500ms
            RGB_LED_set(COLOUR_NIL);

            // take action at here
            // create a motor task and drive the motor according to the cmd
            
            
            cmdr_state = CMDRSTAT_STDBY;
#ifdef SERIAL_DBG_ON    
            Serial.println("lap_cmd is valid, and action should be taken after this ");
#endif
            break;
          }
          // else stay inside the CMDRSTAT_CNFRM state until it timeout or a valid confirmation is received
          vTaskDelay( 250 / portTICK_PERIOD_MS);  // delay 50ms
        }
        else
        {
          diff_ms = millis() - clap_cmd_ms_ts;
#ifdef SERIAL_DBG_ON    
          Serial.print("diff_ms : ");
          Serial.println(diff_ms);
#endif
          if ( diff_ms > CMD_CNFRM_TIMEOUT_MS )
          {
            RGB_LED_set(COLOUR_NIL);
            cmdr_state = CMDRSTAT_STDBY;
            
          }
          vTaskDelay( 250 / portTICK_PERIOD_MS);  // delay 50ms
        }
        break;
    }
  }
}
