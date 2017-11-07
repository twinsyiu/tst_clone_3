#include "motor_ctlr.h"
#include "encoder.h"
#include "UltraSound.h"
#include <Wire.h>   // use I2C library
#include <LiquidCrystal_I2C.h> // // use LiquidCrystal_I2C FC-113, install lib required

float vcc_volt;
const byte ults_interrupt_pin = 3;
int ults_echo_chg_cnt = 0;
unsigned long echo_hi_ts_us = 0;
unsigned long echo_lo_ts_us = 0;
const int buzzerPin = 9;

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup() 
{
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  Serial.print("A0 : ");
  Serial.println(analogRead(A0));
  
  Serial.print("A0 : **** ");
  vcc_volt = analogRead(A0)*5*3/1023;
  Serial.println(vcc_volt);
/*
  if (vcc_volt < 9.0)
  {
    Serial.print("LOW POWER SUPPLY VOLTAGE !!! PLEASE RECHARGE BATTERY NOW ");
    while (1) {};
  }
*/
  ultrasound_init( );
  attachInterrupt(digitalPinToInterrupt(ults_interrupt_pin), ults_echo_chg_isr, CHANGE);
  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  interrupts(); // enable all interrupts
  buzzerPin_setup();
  EE_24C32_setup();

  echo_hi_ts_us = 0;
  echo_lo_ts_us = 0;

  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 

}

// the ISR of ultrasound echo line level change
void ults_echo_chg_isr() 
{
  ults_echo_chg_cnt++;
  if ( digitalRead(com_echoPin) )
  {
    // when the echo pin is HI, record the hi-timestamp
    echo_hi_ts_us = micros();
  } 
  else
  {
    // when the echo pin is LO, record the lo-timestamp
    echo_lo_ts_us = micros();
  }
}


unsigned int ults_cntr;
unsigned int ults_trig = 0;
unsigned int ults_echo_state = 0;

unsigned int r_ults_cntr;
unsigned int r_ults_trig = 0;
unsigned int r_ults_echo_state = 0;

unsigned int l_ults_cntr;
unsigned int l_ults_trig = 0;
unsigned int l_ults_echo_state = 0;


ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
}

float f_dist, l_dist, r_dist;

void loop()
{
  //int ms_ts;
  //long duration;
  String stringOne;
  unsigned int motor_PWM;

//  EE_24C32_loop();
//  song_loop();

  f_dist = ultrasound_read_cm( F_ULTA_ID );
  l_dist = ultrasound_read_cm( L_ULTA_ID );
  r_dist = ultrasound_read_cm( R_ULTA_ID );

 
  stringOne = "left/front/right dist: ";
  stringOne += l_dist; stringOne += " / ";
  stringOne += f_dist; stringOne += " / ";
  stringOne += r_dist;
  Serial.print(stringOne);
  
  
  delay(200);

  lcd.setCursor(0,0); // set the cursor to column 0, line 0
  lcd.print(l_dist); // Print l_dist to the LCD.

  lcd.setCursor(8,0); // set the cursor to column 5, line 0
  lcd.print(f_dist); // Print l_dist to the LCD.

  lcd.setCursor(8,1); // set the cursor to column 10, line 0
  lcd.print(r_dist); // Print l_dist to the LCD.

  motor_PWM = 15;
  if ( f_dist > 30 )
  {
    motor_forward(motor_PWM);
    Serial.println(" move forward");
    lcd.setCursor(0,1); // set the cursor to column 0, line 1
    lcd.print("   F   "); // Print F to the LCD.
    
  } 
  else
  {
    if ( (l_dist < 20) && (r_dist < 20) )
    {
      motor_reverse(motor_PWM + 5);
      Serial.println(" move reverse");
      lcd.setCursor(0,1); // set the cursor to column 0, line 1
      lcd.print("   V   "); // Print F to the LCD.
    }
    else
    if ( l_dist > r_dist )
    {
      motor_turn_left(motor_PWM + 5);
      Serial.println(" turn left");
      lcd.setCursor(0,1); // set the cursor to column 0, line 1
      lcd.print(" <     "); // Print < to the LCD.
      
    }
    else
    {
      motor_turn_right(motor_PWM + 5);
      Serial.println(" turn right");
      lcd.setCursor(0,1); // set the cursor to column 0, line 1
      lcd.print("     > "); // Print < to the LCD.
      
    }
    
    
  }

  
}


//
// previous loop() to test the encoder feedback which adjust the left wheel PWM to ensure straight line movement.
//
void loop_111() 
{
  unsigned int motor_PWM;
  unsigned int l_motor_PWM;
  encoder_Struct encoder;

  int loop_cnt;
  int l_faster, prev_l_faster;

  int i;

  //
  // do motor reverse compensation using the encoder as feedback
  //
  motor_PWM = 15;               // initial test PWM = 15%
  l_motor_PWM = motor_PWM;      // left_motor_PWM = 15%

  // 7 steps to increase the motor PWM
  for ( loop_cnt = 7 ; loop_cnt > 0 ; loop_cnt-- )
  {
    reset_encoder_count();
    
    Serial.print("motor_reverse (%): "); Serial.print(motor_PWM); Serial.print("----- with auto compensation"); 
    motor_reverse(motor_PWM);    // 

    for ( i = 40 ; i > 0 ; i-- )
    {
      delay(200);     // the delay is a test feature/stub to kill time, in real application, other code (FreeRTOS tasks) will be running
      
      // l_faster : an integer flag to indicate if left wheel is faster 
      //   l_faster > 0 : left wheel is faster
      //   l_faster < 0 : left wheel is slower
      
      l_faster = get_encoder( &encoder );
      if ( l_faster > 0 )
      { //   l_faster > 0 : left wheel is faster
        // slow down the left wheel
        if ( prev_l_faster > 0 )
        {
          l_motor_PWM -= 2;     // double the decreament if prev sample indicating the left was faster already
        }
        else
        {
          l_motor_PWM -= 1;
        }
        motor_drive_sp( MOTOR_L, DIR_REV, l_motor_PWM );
        Serial.print("(-) ");
      } 
      else if ( l_faster < 0 )
      { //   l_faster < 0 : left wheel is slower
        // speed up the left wheel
        if ( prev_l_faster < 0 )
        {
          l_motor_PWM += 2;     // double the increament if prev sample indicating the left was slower already
        }
        else
        {
          l_motor_PWM += 1;
        }
        motor_drive_sp( MOTOR_L, DIR_REV, l_motor_PWM );
        Serial.print("(+) ");
      } // else equal, do nothing
      else
      {
        Serial.print("( ) ");
      } // else equal, do nothing
      prev_l_faster = l_faster;
      Serial.print(l_motor_PWM);
      Serial.print("    left enc cnt : "); Serial.print(encoder.l_encoder_count);
      Serial.print("    righ enc cnt : "); Serial.println(encoder.r_encoder_count);
    }
    motor_PWM += 10;
    l_motor_PWM = motor_PWM;
  }

  motor_stop();
  delay(4000);


  
  

  //------------------------------------------------------
  Serial.println("END OF TEST");

  while(1)
  {
  
  
  }
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
