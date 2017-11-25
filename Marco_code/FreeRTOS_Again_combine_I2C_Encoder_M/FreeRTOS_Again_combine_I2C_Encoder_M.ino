#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <NewPing.h>
#include <Wire.h>

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
//// Ultrasound
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

long startMillis_enco = millis();
//long startMillis=millis();
int slot_count_L=0; 
int slot_count_R=0;
 

int clap_counting = 1;
int clap_cnt =0;
long startMillis_ISR=0;
int clap_cnt_command=0;
long startMillis_sys = millis();

int per_cent=256/100;
int set_speed_R= 20*per_cent; //Start up Speed
int set_speed_L=20*per_cent;
int speed_R = set_speed_R;
int speed_L = set_speed_L;
int tot_count_L=0;
int tot_count_R=0;
int car_dir_flag = 0; // 0 = FWD and 1 = REV
int decon_time=1000;

//system Modes
int sys_mode = 1; //0 =auto, 1 = command
int drive_mode = 0; // 1 - FWD
int travel_mode = 0; // 0 - NOT travel_control
  
//SemaphoreHandle_t speed_adj_signal=0;

SemaphoreHandle_t clap_ISR;
 
void setup() 
{
  Serial.begin(9600);
 
  //I2C
  Wire.begin(); // Arduino needs (SDA=A4,SCL=A5)
  address = 0x20;    
  uint8_t bits = B11111111; // All bits off
  
 // set all the motor control pins to outputs
 
  pinMode(IN1, OUTPUT);
  pinMode(IN4, OUTPUT);  
    
  attachInterrupt( digitalPinToInterrupt(2),ISRClapDetect,CHANGE);
  
  vSemaphoreCreateBinary( clap_ISR ); 
    
    // put your setup code here, to run once:
  xTaskCreate( clap_detect, (signed char*)"t1",128,NULL,1,NULL);
 
//  MY NOT USE FOR TESTING xTaskCreate(UltraSound_detect,(signed char*)"Untrasound",128,NULL,1,NULL);
  
//  vSemaphoreCreateBinary(speed_adj_signal); // Give and Take Signal for  Task encoder_read and travel_control
  xTaskCreate(encoder_read,(signed char*)"Encoder",128,NULL,1,NULL);
//  xTaskCreate(travel_control,(signed char*)"Travel",128,NULL,1,NULL);

 
  vTaskStartScheduler();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void ISRClapDetect()
{
/*  < Interrupt Function with debounce >
 *   To stop the sound boundce causing more interrupt & flase ClapCount to occur
  * Millis() timer added
  * startMillis(anytime)-------->Clap detected &millis()& count 1
  *                              startMillis-----within 50S no more count----->if any clap detected & clap_cnt++ 
  *                                                                            startMillis-------50S---> 
*/
 if((millis()-startMillis_ISR)>50)
  {
//    xSemaphoreGiveFromISR(clap_ISR,NULL);
    clap_cnt++;
    startMillis_ISR=millis();
    Serial.print("ISR  ");Serial.println(clap_cnt);
    if (sys_mode==0)
    {
      startMillis_sys=millis();
      sys_mode=1;
      clap_cnt=0;
    }
  }

}

