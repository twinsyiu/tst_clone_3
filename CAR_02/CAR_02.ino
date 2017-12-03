#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required

#include "motor_ctlr.h"
#include "encoder.h"
#include "UltraSound.h"
#include <Wire.h>   // use I2C library
#include <LiquidCrystal_I2C.h> // // use LiquidCrystal_I2C FC-113, install lib required

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// The followings are defined movement state
enum movement_st
{
  MVSTATE_STOP,
  MVSTATE_GO_FWD,
  MVSTATE_TURN_LEFT,
  MVSTATE_TURN_RIGHT,
  MVSTATE_KEEP_LEFT,
  MVSTATE_KEEP_RIGHT,
  MVSTATE_REV,
  MVSTATE_PAUSE,
  MVSTATE_DEAD_STOP // for low battery voltage
};

// default movement_state when robot start
unsigned int movement_state = MVSTATE_STOP;


// the followings are defined RGB_LED colours
enum LED_COLOR
{
  R_WH = 1,
  R_RD,
  R_GN,
  R_BL,
  L_RD_R_WH,
  L_RD_R_RD,
  L_RD_R_GN,
  L_RD_R_BL,
  L_GN_R_GN,
  L_GN_R_BL,
  L_BL_R_BL,
  L_RD,
  L_GN,
  L_BL,
  L_BL_R_GN,
  L_GN_R_RD,
  L_WH_R_WH,
  ALL_OFF
};

// ultrasound sensors measured distance from objects
int f_dist, l_dist, r_dist;

// initial general motor PWM in %, ie 25%
unsigned int motor_PWM = 25;
// declare specific left & right motor PWM variables
unsigned int l_motor_PWM, r_motor_PWM;


/*
 * the main setup()
 */
void setup()
{
  float vcc_volt;

  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  lcd.init(); //initialize the I2C LCD
  lcd.backlight(); //turn on the backlight

  Init_RGBLED();  // initialise the RGB LED driven by the I2C 8bit IO Port expansion board
  I2C_RGB_LED(L_WH_R_WH);

  // get the battery terminal voltage
  vcc_volt = get_battery_voltage();
  // check if the battery terminal voltage is above 9.5V
  battery_low_hdlr( vcc_volt );
  // display the terminal voltage on LCD
  display_battery(vcc_volt);
  // pause for 1 seconds
  delay(1000);

  lcd.clear(); // clear the lcd

  // initialise the ultrasound sensors
  ultrasound_init();

  // initialise the Clap Detection task
  TaskClapDetect_init();

  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  interrupts(); // enable all interrupts

  // initailise the buzzer
  buzzerPin_setup();

  // initialise motion handler
  init_motion_handler();

  // Set up speedo Task to handle ultrasound distance measurement.
  xTaskCreate(
    Task_Speedo
    ,  (const portCHAR *)"Task_Speedo"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );

  I2C_RGB_LED(ALL_OFF);

}

/*
 * researved timer 1 isr handler
ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
}
*/

// main loop :
// every handler shall look after its own timing to wake up and
// work on its own tasks; they shuold avoid to use delay() unless it is prtotyping
// or external source
void loop()
{
    dislplay_to_LCD();
    motion_handler();
    motion_stuck_handler();
    check_battery_hdlr();
    clap_cmd_hdlr();
}

// Interrupt is called on every milli-second
SIGNAL(TIMER0_COMPA_vect)
{
  encoder_update();
}

unsigned long next_check_battery_time_ms = 0;

/*
 * get_battery_voltage() reads analog input and returns the converted battery terminal voltage
 */
float get_battery_voltage (void)
{
  float vcc_volt;

  // A0 pin is connected to the voltage divider of the terminal voltage
  // full scale 1023 is representing 15V DC
  vcc_volt = analogRead(A0) * 5 * 3 / 1023;

  return vcc_volt;
}

/*
 * battery_low_hdlr() puts the system in halt when battery terminal voltage is less than the
 * 9.5V minimum, except that when it is connected and powered by the USB during software development
 */
void battery_low_hdlr (float in_volt)
{
  if (in_volt < 0.2)
  { // ie, it is powered by USB, so skip the rest of the test
    return;
  }

  if (in_volt < 9.5)
  {
    lcd.init(); //initialize the lcd
    lcd.backlight(); //open the backlight
    // Serial.print("LOW POWER SUPPLY VOLTAGE !!! PLEASE RECHARGE BATTERY NOW ");
    lcd.setCursor(0,0); // set the cursor to column 0, line 0
    lcd.print("BATTERY LOW"); // Print l_dist to the LCD.
    lcd.setCursor(0,1); // set the cursor to column 0, line 0
    lcd.print(in_volt); // Print l_dist to the LCD.
    lcd.setCursor(6,1); // set the cursor to column 0, line 0
    lcd.print("Volt"); // Print l_dist to the LCD.

    while (1) {};
  }
}

/*
 * check_battery_hdlr() will wake up every second and check if the battery voltage is healthy
 */
void check_battery_hdlr (void)
{
  float vcc_volt;
  unsigned long current_time;

  current_time = millis();
  if ( current_time < next_check_battery_time_ms )
    return;   // skip the rest if time has not expired

  next_check_battery_time_ms = current_time + 1000;
//  Serial.print(current_time);Serial.print(" --bb-- ");Serial.println(next_check_battery_time_ms);

  // A0 pin is connected to the voltage divider of the terminal voltage
  // full scale 1023 is representing 15V DC
  vcc_volt = get_battery_voltage();
  battery_low_hdlr( vcc_volt );

  return;
}


