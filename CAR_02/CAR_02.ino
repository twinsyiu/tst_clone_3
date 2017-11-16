#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include "EE_24C32.h"

#include "motor_ctlr.h"
#include "encoder.h"
#include "UltraSound.h"
#include <Wire.h>   // use I2C library
#include <LiquidCrystal_I2C.h> // // use LiquidCrystal_I2C FC-113, install lib required

float vcc_volt;
/*
const byte ults_interrupt_pin = 3;
int ults_echo_chg_cnt = 0;
unsigned long echo_hi_ts_us = 0;
unsigned long echo_lo_ts_us = 0;
*/
const int buzzerPin = 9;

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

enum movement_st 
{
  MVSTATE_STOP,
  MVSTATE_GO_FWD,
  MVSTATE_TURN_LEFT,
  MVSTATE_TURN_RIGHT,
  MVSTATE_LEFT_90,
  MVSTATE_RIGHT_90,
  MVSTATE_TURN_180,
  MVSTATE_TRAPPED,
  MVSTATE_KEEP_LEFT,
  MVSTATE_KEEP_RIGHT
};



void setup() 
{
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  Serial.print("A0 : ");
  Serial.println(analogRead(A0));
  
  Serial.print("A0 : **** ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);

  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 

  if (vcc_volt < 9.5)
  {
    Serial.print("LOW POWER SUPPLY VOLTAGE !!! PLEASE RECHARGE BATTERY NOW ");
    lcd.setCursor(0,0); // set the cursor to column 0, line 0
    lcd.print("BATTERY LOW"); // Print l_dist to the LCD.
    lcd.setCursor(0,1); // set the cursor to column 0, line 0
    lcd.print(vcc_volt); // Print l_dist to the LCD.
    lcd.setCursor(6,1); // set the cursor to column 0, line 0
    lcd.print("Volt"); // Print l_dist to the LCD.
//    while (1) {};
  }
  else
  {
    lcd.setCursor(0,0); // set the cursor to column 0, line 0
    lcd.print("MOTOR DISABLE TST"); // Print l_dist to the LCD.
    lcd.setCursor(0,1); // set the cursor to column 0, line 0
    lcd.print(vcc_volt); // Print l_dist to the LCD.
    lcd.setCursor(6,1); // set the cursor to column 0, line 0
    lcd.print("Volt"); // Print l_dist to the LCD.
  }
  delay(1000);

  //lcd.init(); //initialize the lcd
  
  ultrasound_init( );

  
  
  // attachInterrupt(digitalPinToInterrupt(ults_interrupt_pin), ults_echo_chg_isr, CHANGE);
  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  interrupts(); // enable all interrupts
  buzzerPin_setup();
  EE_24C32_setup();

  echo_hi_ts_us = 0;
  echo_lo_ts_us = 0;

  // Now set up speedo Task to run independently.
  xTaskCreate(
    Task_Speedo
    ,  (const portCHAR *)"Task_Speedo"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );



}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
}

float f_dist, l_dist, r_dist, tg_f, tg_l, tg_r;

void loop()
{
  //int ms_ts;
  //long duration;
//  String stringOne;
  unsigned int motor_PWM, movement_state, l_motor_PWM, r_motor_PWM;
  encoder_Struct encoder;
  int l_faster, prev_l_faster;

//  EE_24C32_loop();
//  song_loop();

  int change_state_ms_ts;

  // determine terrain state
  // if ( f_dist > 30 )
  movement_state = MVSTATE_STOP;
  motor_PWM = 20;
  
  while (1)
  {
    switch (movement_state)
    {
      case MVSTATE_STOP:
        //Serial.println(" MVST_STOP");
        lcd.setCursor(0,0); // set the cursor to column 0, line 0
        lcd.print("                "); // Print < to the LCD.
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("   0            "); // Print < to the LCD.
        delay(200);

        if ( f_dist > 40 )
        {
          //Serial.println("_FWD");
          movement_state = MVSTATE_GO_FWD;
          motor_forward( motor_PWM );
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
        }
        else if ( (l_dist > r_dist) && (l_dist > 40) )
        {
          //Serial.println("TN_L");
          movement_state = MVSTATE_TURN_LEFT;
          change_state_ms_ts = millis();
          motor_PWM = 16;
          motor_turn_left( motor_PWM );
        }
        else if ( r_dist > 40 )
        {
          //Serial.println("TN_R");
          movement_state = MVSTATE_TURN_RIGHT;
          change_state_ms_ts = millis();
          motor_PWM = 16;
          motor_turn_right( motor_PWM );
        }
        else
        {
          //Serial.println("TRAP");
          movement_state = MVSTATE_TRAPPED;
          change_state_ms_ts = millis();
          motor_reverse( motor_PWM );
        }
        delay(200);
        break;

      case MVSTATE_GO_FWD:
        //Serial.println(" MVST_FWD");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("   ^   "); // Print < to the LCD.
        delay(200);

        // try to go straight, adjust pwm if necessary
        // =============================================
        l_faster = get_encoder( &encoder );
        
//        Serial.print("= left enc cnt : "); Serial.print(encoder.l_encoder_count);
//        Serial.print(" right enc cnt : "); Serial.print(encoder.r_encoder_count);
//        Serial.print(" :: ");
         
        if ( l_faster > 0 )
        { // l_faster > 0 : left wheel is faster
          // compensate by slow down the left wheel
          if ( prev_l_faster > 0 )
          {
            if ( l_motor_PWM > 12 )
              l_motor_PWM -= 2;     // double the decreament if prev sample indicating the left was faster already
            else
              if ( (r_motor_PWM >= motor_PWM) && ((r_motor_PWM - motor_PWM) < 10) )
                r_motor_PWM += 2;
          }
          else
          {
            if ( l_motor_PWM > 11 )
              l_motor_PWM -= 1;
            else
              if ( (r_motor_PWM >= motor_PWM) && ((r_motor_PWM - motor_PWM) < 10) )
                r_motor_PWM += 1;
          }
          
//          Serial.print("(-) ");
        } 
        else if ( l_faster < 0 )
        { //   l_faster < 0 : left wheel is slower
          // speed up the left wheel
          if ( prev_l_faster < 0 )
          {
            l_motor_PWM += 2;     // double the increament if prev sample indicating the left was slower 
            if ( (l_motor_PWM - motor_PWM) > 10 )
            {
              l_motor_PWM -= 2;
              r_motor_PWM -= 2;
            }            
          }
          else
          {
            l_motor_PWM += 1;
            if ( (l_motor_PWM - motor_PWM) > 10 )
            {
              l_motor_PWM -= 1;
              r_motor_PWM -= 1;
            }            

          }
//          Serial.print("(+) ");
        } // else equal, do nothing
        else
        { // l_faster is 0 ; ie the encoder counters are equal, the best time to reset the counters
//          Serial.print("( ) ");
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
        } // else equal, do nothing
        PWM_adjust (&l_motor_PWM, &r_motor_PWM, 5, motor_PWM);

        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );

        
        prev_l_faster = l_faster;
        //Serial.print(l_motor_PWM); Serial.print(" | "); Serial.println(r_motor_PWM);
//        Serial.print("    left enc cnt : "); Serial.print(encoder.l_encoder_count);
//        Serial.print("    righ enc cnt : "); Serial.println(encoder.r_encoder_count);

        delay(200);


        if ( f_dist < 40 )
        {
          if (( l_dist >= r_dist ) && ( l_dist > 40 ))
          {
            tg_r = f_dist;
            tg_f = l_dist;
            motor_turn_left( motor_PWM - 5 );
            movement_state = MVSTATE_LEFT_90;
          }
          else if ( r_dist > 40 )
          {
            tg_l = f_dist;
            tg_f = r_dist;
            motor_turn_right( motor_PWM - 5 );
            movement_state = MVSTATE_RIGHT_90;
          }
          else
          {
            tg_l = r_dist;
            tg_r = l_dist;
            motor_turn_right( motor_PWM - 5 );
            movement_state = MVSTATE_RIGHT_90;  // use right turn to achieve the 180 degree turn 
          }
          change_state_ms_ts = millis();
        } 
        else if ( l_dist < 30 )
        {
          //motor_turn_right( motor_PWM );
          movement_state = MVSTATE_KEEP_LEFT;
          reset_encoder_count();
          l_motor_PWM = motor_PWM + 2;
          r_motor_PWM = motor_PWM - 2;
          motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
          motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
          change_state_ms_ts = millis();
        } 
        else if ( r_dist < 30 )
        {
          // motor_turn_left( motor_PWM );
          movement_state = MVSTATE_KEEP_RIGHT;
          reset_encoder_count();
          l_motor_PWM = motor_PWM - 2;
          r_motor_PWM = motor_PWM + 2;
          motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
          motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
          change_state_ms_ts = millis();
        }
        delay(200);

        // detect if stucked
        
        break;

      //case MVSTATE_TURN_LEFT:
      case MVSTATE_KEEP_LEFT:
        //Serial.println(" MVST_KP_L");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("   |>  "); // Print < to the LCD.
        delay(200);

        if ( f_dist < 40 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          continue;
        }

        if ( l_dist > 40 )
        {
          l_motor_PWM -= 1;
          r_motor_PWM += 1;
          if ( l_motor_PWM < 15 )
            l_motor_PWM = 15;
        } else if ( l_dist > 35 )
        {
          motor_forward( motor_PWM );
          l_motor_PWM = r_motor_PWM = motor_PWM;
          reset_encoder_count();
          prev_l_faster = 0;
        } else if ( l_dist < 30 )
        {
          l_motor_PWM += 1;
          r_motor_PWM -= 1;
          if ( r_motor_PWM < 15 )
            r_motor_PWM = 15;
        }
        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );
          
        // detect if stucked
        delay(200);

        break;
        
      //case MVSTATE_TURN_RIGHT:
      case MVSTATE_KEEP_RIGHT:
        //Serial.println(" MVST_KP_R");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("  <|   "); // Print < to the LCD.
        delay(200);

        if ( f_dist < 40 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          continue;
        }
        if ( r_dist > 40 )
        {
          r_motor_PWM -= 1;
          l_motor_PWM += 1;
          if ( r_motor_PWM < 15 )
            r_motor_PWM = 15;
        } else if ( r_dist > 35 )
        {  
          motor_forward( motor_PWM );
          l_motor_PWM = r_motor_PWM = motor_PWM;
          reset_encoder_count();
          prev_l_faster = 0;
          
        } else if ( r_dist < 30 )
        {
          r_motor_PWM += 1;
          l_motor_PWM -= 1;
          if ( l_motor_PWM < 15 )
            l_motor_PWM = 15;
        }
        motor_drive_sp( MOTOR_L, DIR_FWD, l_motor_PWM );
        motor_drive_sp( MOTOR_R, DIR_FWD, r_motor_PWM );

        // detect if stucked
        delay(200);

        break;

      case MVSTATE_TURN_LEFT:
      //case 100:
        //Serial.println(" MVST_TN_L");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("  <    "); // Print < to the LCD.
        //delay(200);

        if ( f_dist > 40 )
        {
          motor_PWM = 20;
          motor_forward( motor_PWM );
          movement_state = MVSTATE_GO_FWD;
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
          continue;
        } else if ( ( millis() - change_state_ms_ts ) > 500 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          continue;
        }

        if ( l_dist < 30 )
        {
          if ( r_dist < 30 )
          {
            movement_state = MVSTATE_TRAPPED;
            motor_reverse( motor_PWM );
            continue;
          }
          movement_state = MVSTATE_TURN_RIGHT;
          change_state_ms_ts = millis();
          motor_PWM = 16;
          motor_turn_right( motor_PWM );
          
        }
        break;

      //case 101:
      case MVSTATE_TURN_RIGHT:
        //Serial.println(" MVST_TN_R");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("    >  "); // Print < to the LCD.
        //delay(200);

        if ( f_dist > 40 )
        {
          motor_PWM = 20;
          motor_forward( motor_PWM );
          movement_state = MVSTATE_GO_FWD;
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
          continue;
        } else if ( ( millis() - change_state_ms_ts ) > 500 )
        {
          motor_stop();
          movement_state = MVSTATE_STOP;
          change_state_ms_ts = millis();
          continue;
        }

        if ( r_dist < 30 )
        {
          if ( l_dist < 30 )
          {
            movement_state = MVSTATE_TRAPPED;
            motor_reverse( motor_PWM );
            continue;
          }
          movement_state = MVSTATE_TURN_LEFT;
          change_state_ms_ts = millis();
          motor_PWM = 16;
          motor_turn_left( motor_PWM );
          
        }
        break;

      case MVSTATE_LEFT_90:
        //Serial.println(" MVST_L_90");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print(" [<    "); // Print < to the LCD.

        //if ( close_to ( r_dist, tg_r, 5 ) && close_to ( f_dist, tg_f, 5 ) )
        if ( f_dist > 40 )
        {
          motor_forward( motor_PWM );
          movement_state = MVSTATE_GO_FWD;
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
        } else if ( ( millis() - change_state_ms_ts ) > 500 )
        {
          motor_stop();
          change_state_ms_ts = millis();
          movement_state = MVSTATE_STOP;
        }

        delay(200);

        break;
        
      case MVSTATE_RIGHT_90:
        //Serial.println(" MVST_R_90");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("    >] "); // Print < to the LCD.

        //if ( close_to ( l_dist, tg_l, 5 ) && close_to ( f_dist, tg_f, 5 ) )
        if ( f_dist > 40 )
        {
          motor_forward( motor_PWM );
          movement_state = MVSTATE_GO_FWD;
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
        } else if ( ( millis() - change_state_ms_ts ) > 500 )
        {
          motor_stop();
          change_state_ms_ts = millis();
          movement_state = MVSTATE_STOP;
        }

        delay(200);
        break;
        
      case MVSTATE_TURN_180:
        //Serial.println(" MVST_T180");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print("   v   "); // Print < to the LCD.
        delay(200);

        if ( close_to ( l_dist, tg_l, 5 ) && close_to ( r_dist, tg_r, 5 ) )
        {
          motor_forward( motor_PWM );
          movement_state = MVSTATE_GO_FWD;
          reset_encoder_count();
          prev_l_faster = 0;
          l_motor_PWM = r_motor_PWM = motor_PWM;
        } else if ( ( millis() - change_state_ms_ts ) > 2000 )
        {
          motor_stop();
          change_state_ms_ts = millis();
          movement_state = MVSTATE_STOP;
        }

        break;
        
      case MVSTATE_TRAPPED:
      default:
        //Serial.println("MVST_TRAP");
        lcd.setCursor(0,1); // set the cursor to column 0, line 1
        lcd.print(" ||=|| "); // Print < to the LCD.
        delay(200);
       if ( l_dist > 30 )
        {
          change_state_ms_ts = millis();
          movement_state = MVSTATE_TURN_LEFT;
          motor_PWM = 16;
          motor_turn_left( motor_PWM );
          continue;
        }
        if ( r_dist > 30 )
        {
          change_state_ms_ts = millis();
          movement_state = MVSTATE_TURN_RIGHT;
          motor_PWM = 16;
          motor_turn_right( motor_PWM );
          continue;
          
        }

        break;
    }
  }
}

bool close_to ( float a, float b, int percent )
{
  float diff;
  if ( a < 0 || b < 0 )
    return false;
  if ( a == b )
    return true;
    
  if ( b > a )
  {
    diff = (b - a) * 100 / b ;
  }
  else
  {
    diff = (a - b) * 100 / a ;
  }

  if ( diff < percent )
  {
    return true;
  }
  return false;
}


// Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) 
{

  encoder_update();

}

/*
SparkFun Inventor's Kit 
Example sketch 11

BUZZER

  Use the buzzer to play a song!

  The buzzer in your Inventor's Kit is an electromechanical
  component you can use to make noise. Inside the buzzer is a
  coil of wire and a small magnet. When current flows through 
  the coil, it becomes magnetized and pulls towards the magnet,
  creating a tiny "click". When you do this thousands of times
  per second, you create tones.

  The Arduino has a built-in command called tone() which clicks
  the buzzer at a certain frequency. This sketch knows the
  frequencies of the common notes, allowing you to create songs.
  We're never going to let you down!

Hardware connections:

  The buzzer has two pins. One is positive and one is negative.
  The postitive pin is marked by a "+" symbol on both the top
  and bottom of the buzzer.

  Connect the positive pin to Arduino digital pin 9.
  (Note that this must be a PWM pin.)
  Connect the negative pin to GND.

  Tip: if the buzzer doesn't fit into the breadboard easily,
  try rotating it slightly to fit into diagonal holes.

This sketch was written by SparkFun Electronics,
with lots of help from the Arduino community.
(This sketch was originally developed by D. Cuartielles for K3)
This code is completely free for any use.
Visit http://learn.sparkfun.com/products/2 for SIK information.
Visit http://www.arduino.cc to learn about the Arduino.

Version 2.0 6/2012 MDG
*/

/*
This sketch uses the buzzer to play songs.
The Arduino's tone() command will play notes of a given frequency.
We'll provide a function that takes in note characters (a-g),
and returns the corresponding frequency from this table:

  note  frequency
  c     262 Hz
  d     294 Hz
  e     330 Hz
  f     349 Hz
  g     392 Hz
  a     440 Hz
  b     494 Hz
  C     523 Hz

For more information, see http://arduino.cc/en/Tutorial/Tone
*/

//const int buzzerPin = 9;

// We'll set up an array with the notes we want to play
// change these values to make different songs!

// Length must equal the total number of notes and spaces 

const int songLength = 18;

// Notes is an array of text characters corresponding to the notes
// in your song. A space represents a rest (no tone)

char notes[] = "cdfda ag cdfdg gf "; // a space represents a rest

// Beats is an array of values for each note and rest.
// A "1" represents a quarter-note, 2 a half-note, etc.
// Don't forget that the rests (spaces) need a length as well.

int beats[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};

// The tempo is how fast to play the song.
// To make the song play faster, decrease this value.

int tempo = 113;


void buzzerPin_setup() 
{
  pinMode(buzzerPin, OUTPUT);
}


void song_loop() 
{
  int i, duration;

  for (i = 0; i < songLength; i++) // step through the song arrays
  {
    duration = beats[i] * tempo;  // length of note/rest in ms

    if (notes[i] == ' ')          // is this a rest? 
    {
      delay(duration);            // then pause for a moment
    }
    else                          // otherwise, play the note
    {
      tone(buzzerPin, frequency(notes[i]), duration);
      delay(duration);            // wait for tone to finish
    }
    delay(tempo/10);              // brief pause between notes
  }

  // We only want to play the song once, so we'll pause forever:
//  while(true){}
  // If you'd like your song to play over and over,
  // remove the above statement
}


int frequency(char note) 
{
  // This function takes a note character (a-g), and returns the
  // corresponding frequency in Hz for the tone() function.

  int i;
  const int numNotes = 8;  // number of notes we're storing

  // The following arrays hold the note characters and their
  // corresponding frequencies. The last "C" note is uppercase
  // to separate it from the first lowercase "c". If you want to
  // add more notes, you'll need to use unique characters.

  // For the "char" (character) type, we put single characters
  // in single quotes.

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.

  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return(frequencies[i]);     // Yes! Return the frequency
    }
  }
  return(0);  // We looked through everything and didn't find it,
              // but we still need to return a value, so return 0.
}

// PWM_adjust() will bring the l_PWM & r_PWM back to the PWM_centre when there is small diff between the l & r PWM
void PWM_adjust(unsigned int * l_PWM, unsigned int * r_PWM, unsigned int PWM_diff_margin, unsigned int PWM_centre)
{
  // if the diff between l_PWM and R_PWM is less than PWM_diff_margin, 
  // then l_PWM will be set to PWM_centre and r_PWM will be adjusted with the diff
  unsigned int l_r_diff;
  
  if ( *l_PWM >= *r_PWM )
  {
    l_r_diff = *l_PWM - *r_PWM;
    if ( (l_r_diff < PWM_diff_margin) && (*l_PWM != PWM_centre) )
    {
      *l_PWM = PWM_centre;
      *r_PWM = PWM_centre - l_r_diff;
    }
  }
  else
  {
    l_r_diff = *r_PWM - *l_PWM;
    if ( (l_r_diff < PWM_diff_margin) && (*l_PWM != PWM_centre) )
    {
      *l_PWM = PWM_centre;
      *r_PWM = PWM_centre + l_r_diff;
    }
    
  }
}

float ultr_dist[3];

void Task_Speedo( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  int ulta_expire_time_ms;    // when no ultrasound echo within this time, it will expire
  unsigned long start_trig_ts_ms, start_measure_ts_ms;
  String stringOne;

//Serial.println("Tk_Spdo 1");

  for (;;) // A Task shall never return or exit.
  {
    start_measure_ts_ms = millis();
    for ( int ulta_id = 0; ulta_id <= R_ULTA_ID; ulta_id++ )
    {
      ultr_dist[ulta_id] = 0.0;
      start_trig_ts_ms = millis();
      ultrasound_trigger( ulta_id );
      do {
        vTaskDelay(30 / portTICK_PERIOD_MS);  // delay 15ms
        if ( echo_hi_ts_us && echo_lo_ts_us  && (echo_lo_ts_us > echo_hi_ts_us ) && !digitalRead(com_echoPin) )
        {
          // this is a valid return from the ultrasound sensor
          //Serial.print("front dist: "); Serial.println( constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399) );
          // max 399cm ie 
          ultr_dist[ulta_id] = constrain((echo_lo_ts_us - echo_hi_ts_us) * 3.43 / 200 , 0 , 399);
          break;  // break the do-while loop
        }
      }
      while ( (ultr_dist[ulta_id] <= 0) && (( millis() - start_trig_ts_ms ) < 45 ) );    // 
    }
    
    f_dist = ultr_dist[F_ULTA_ID];
    l_dist = ultr_dist[L_ULTA_ID];
    r_dist = ultr_dist[R_ULTA_ID];
   
    lcd.setCursor(0,0); // set the cursor to column 0, line 0
    lcd.print(l_dist); // Print l_dist to the LCD.
  
    lcd.setCursor(8,0); // set the cursor to column 5, line 0
    lcd.print(f_dist); // Print l_dist to the LCD.
  
    lcd.setCursor(8,1); // set the cursor to column 10, line 0
    lcd.print(r_dist); // Print l_dist to the LCD.

    vTaskDelay(200 / portTICK_PERIOD_MS);  // nof tick delay (45ms)
  }
}

