#ifndef CLAP_DETECT_H
#define CLAP_DETECT_H

#define CLAP_DEBOUNCE_MS     30
#define CLAP_CMD_PAUSE_US   550000

typedef void* QueueHandle_t;

const byte interruptPin = 2;

bool clap_detected_f = false;
unsigned long clap_start_us;    // clap detected time
bool clap_valid_f = false;
unsigned int clap_valid_cnt = 0;

QueueHandle_t qu_clap;

void TaskClapDetect( void *pvParameters );
void TaskClapDetect_init( void );

#endif
