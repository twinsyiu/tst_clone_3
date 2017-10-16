#include <Arduino_FreeRTOS.h>
#include <semphr.h>


// Encoder Hc-20K
#define enco_pin_R 12  //left  motor encoder
#define enco_pin_L 13  // right motor encoder
//I2C
uint8_t address;


// motor one
 
#define IN2  6
#define IN1  7
// motor two
 
#define IN4  4
#define IN3  5


 
 

long startMillis_enco = millis();
//long startMillis=millis();
int slot_count_L=0;
 
int slot_count_R=0;
 
  
  int per_cent=256/100;
  int set_speed_R= 30*per_cent; //Start up Speed
  int set_speed_L=32*per_cent;
  int tot_count_L=0;
  int tot_count_R=0;
  int car_dir_flag = 0; // 0 = FWD and 1 = REV
  int enco_count_time=1000;
  
SemaphoreHandle_t speed_adj_signal=0;
void setup() 
{
  Serial.begin(9600);
  delay(3000);
  
 // set all the motor control pins to outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN4, OUTPUT);  
  
  vSemaphoreCreateBinary(speed_adj_signal); 
  xTaskCreate(encoder_read,(signed char*)"Untrasound",128,NULL,1,NULL);
  xTaskCreate(travel_control,(signed char*)"Untrasound",128,NULL,1,NULL);
  vTaskStartScheduler();

 
}

void loop() 
{



}


