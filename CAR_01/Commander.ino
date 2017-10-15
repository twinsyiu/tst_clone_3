#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include "Commander.h"
#include "ClapDetect.h"
#include "RGBLED_Hdlr.h"
#include "motor_ctlr.h"

#define CMDRSTAT_STDBY     0
#define CMDRSTAT_CNFRM     1

#define CMD_CNFRM_TIMEOUT_MS     4000

#define MOTION_STAT_HALT          0
#define MOTION_STAT_MOVING_FWD    1
#define MOTION_STAT_MOVING_RVD    2


unsigned int clap_cmd = 0;
bool clap_cmd_rdy_f = false;
bool clap_cmd_pending_f = false;

bool motor_running_f;

void TaskCommander_init( void )
{
  // using RISING edge on sound level sensor
  // attachInterrupt(digitalPinToInterrupt(interruptPin), log_chg, RISING);
  RGB_LED_set(COLOUR_NIL);
  motor_running_f = false;
}

void TaskCommander( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  unsigned int cmdr_state = CMDRSTAT_STDBY;    // waiting a clap
  unsigned int clap_cmd_ms_ts, diff_ms;

  unsigned int clap_cnt_msg;
  
  for (;;)
  {
    switch (cmdr_state)
    {
      case CMDRSTAT_STDBY:
        if (clap_valid_f)
        {
          clap_valid_f = false;     // clear the valid flag
          clap_cmd = clap_valid_cnt;
          Serial.print(" PENDING clap_cmd : ");
          Serial.println(clap_cmd);
          // display RGB
          RGB_LED_set(colour_arry[constrain(clap_valid_cnt,0,4)]);

          cmdr_state = CMDRSTAT_CNFRM;
          Serial.print("CHG cmdr_state : ");
          Serial.println(cmdr_state);
          clap_cmd_ms_ts = millis();
         
        }

        vTaskDelay( 50 / portTICK_PERIOD_MS);  // delay 50ms
        break;
      case CMDRSTAT_CNFRM:
        if (clap_valid_f)
        {
          // Serial.print("clap_valid_f : TRUE ======= clap_valid_cnt : ");
          // Serial.println(clap_valid_cnt);
          clap_valid_f = false;     // clear the valid flag
          if (clap_valid_cnt == 1)
          {
            clap_valid_cnt = 0;
            
            // clap_cmd is valid, blink to confirm the Confirmation clap is received
            RGB_LED_set(COLOUR_NIL);
            vTaskDelay( 400 / portTICK_PERIOD_MS);  // delay 450ms
            RGB_LED_set(colour_arry[constrain(clap_cmd,0,4)]);
            vTaskDelay( 500 / portTICK_PERIOD_MS);  // delay 500ms
            RGB_LED_set(COLOUR_NIL);

            clap_cmd_rdy_f = true;
            // take action at here
            // create a motor task and drive the motor according to the cmd
            //motion_control(clap_cmd);
            
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
            clap_valid_cnt = 0;
            clap_cmd_rdy_f = false;
          }
          vTaskDelay( 250 / portTICK_PERIOD_MS);  // delay 50ms
        }
        break;
    }
  }
}

void TaskMotionCtrl( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  unsigned int motion_state = MOTION_STAT_HALT;    
  unsigned int motor_PWM;
  
  for (;;)
  {
    // Serial.print(millis());
    // Serial.println(" - TaskMotionCtrl");
    switch (motion_state)
    {
      case MOTION_STAT_HALT:
        // listen to command
        if ( clap_cmd_rdy_f )
        {
          clap_cmd_pending_f = false;
          clap_cmd_rdy_f = false;
          switch (clap_cmd)
          {
            case 1:     // GO FORWARD
              //Serial.println("motion state : MOTION_STAT_HALT --> MOTION_STAT_MOVING_FWD");
              motor_PWM = 100;
              motor_forward(motor_PWM);    // 50%
              motor_running_f = true;
              motion_state = MOTION_STAT_MOVING_FWD;
              clap_cmd_pending_f = false;
              break;
            case 2:     // GO BACKWARD
              //Serial.println("motion state : MOTION_STAT_HALT --> BACKWARD ");
              motor_PWM = 100;
              motor_reverse(motor_PWM);    // 50%
              motor_running_f = true;
              clap_cmd_pending_f = false;
              motion_state = MOTION_STAT_MOVING_RVD;
              break;
            case 3:     // BACK OFF A BIT
              //Serial.println("motion state : MOTION_STAT_HALT --> BACKOFF --> MOTION_STAT_MOVING_FWD");
              motor_reverse(100);    // 50%
              vTaskDelay( 2000 / portTICK_PERIOD_MS);
              motor_stop();
              motor_running_f = false;
              clap_cmd_pending_f = false;
              motion_state = MOTION_STAT_HALT;
              break;
          }
        }
        vTaskDelay( 30 / portTICK_PERIOD_MS);
        break;
      case MOTION_STAT_MOVING_FWD:
      case MOTION_STAT_MOVING_RVD:
        // check ultrasound sensor distance
        // listen to command
        if ( clap_cmd_rdy_f )
        {
          clap_cmd_pending_f = false;
          clap_cmd_rdy_f = false;
          switch (clap_cmd)
          {
            case 1: // SPEED UP half of current PWM
              motor_PWM = constrain(motor_PWM * 1.5, 0, 100);
              //Serial.println("motion state : MOTION_STAT_MOVING_FWD --> 3 claps --> increase speed by 50%");
              Serial.print("in motion, 1 clap: "); Serial.println(motor_PWM);
              goto APPLY_PWM;
              break;
            case 2: // SLOW DOWN to half of current PWM
              //Serial.println("motion state : MOTION_STAT_MOVING_FWD --> 1 clap --> reduce speed by 50%");
              motor_PWM = constrain(motor_PWM * 0.9, 0, 100);
              Serial.print("in motion, 2 claps: "); Serial.println(motor_PWM);
APPLY_PWM:              
              if ( motion_state == MOTION_STAT_MOVING_FWD )
              {
                motor_forward(motor_PWM); 
                //break;
              }
              else if ( motion_state == MOTION_STAT_MOVING_RVD )
              {
                motor_reverse(motor_PWM); 
                //break;
              }
                 
              break;
            case 3:
              motor_PWM = 0;
              Serial.print("in motion, 3 claps: "); Serial.println(motor_PWM);
              motor_stop();
              //Serial.println("motion state : MOTION_STAT_MOVING_FWD --> 2 claps --> MOTION_STAT_HALT");
              motor_running_f = false;
              motion_state = MOTION_STAT_HALT;
              break;
          }
        }
        
        // check obstacle
        if (obstacle_distance_cm < 15)
        {
          motor_stop();
          motor_running_f = false;
          motion_state = MOTION_STAT_HALT;
        } else
        if ( (obstacle_distance_cm < 50) && (motor_PWM >= 50) )
        {
          motor_PWM = constrain(motor_PWM * 0.5, 0, 100);
          motor_forward(motor_PWM);    
        }

        vTaskDelay( 230 / portTICK_PERIOD_MS);  //30 before
        break;
    }
  }
}

void TaskMotionCtrl_init( void )
{
  motor_stop();
}

