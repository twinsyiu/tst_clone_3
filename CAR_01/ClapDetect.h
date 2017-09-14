#ifndef CLAP_DETECT_H
#define CLAP_DETECT_H

#define CLAP_DEBOUNCE_MS     30
#define CLAP_CMD_PAUSE_US   550000

const byte interruptPin = 2;

bool clap_detected_f = false;
unsigned long clap_start_us;    // clap detected time
bool clap_valid_f = false;
unsigned int clap_valid_cnt = 0;

void TaskClapDetect( void *pvParameters );
//void TaskClapDetect( void *pvParameters __attribute__((unused)) );  // This is a Task.
void TaskClapDetect_init( void );

#endif
