// This program sourced from
// https://www.arduino.cc/en/Reference/AttachInterrupt
// to test the interrupt pin 
// NOTES: The gain of the audio sensor is tuned CCW to very low until the RED LED on the module is just OFF

// Sound module
// DO pin --> Arduino Pin 2 (interrupt_0)

// LEDGENT:
// _ts means time_stamp

#define SINGLE_CLAP_SILENT_USEC 8000
#define ALL_CLAP_SILENT_USEC 450000

const byte ledPin = 5;
const byte interruptPin = 2;

unsigned long first_rise_ts = 0;
unsigned long last_fall_ts = 0;

// TY: I am dis-like these two needed to be declared as global variable
unsigned long last_clap_fall_ts = 0;
unsigned int clap_cnt = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("setup() - begin");
  pinMode(ledPin, OUTPUT);

  // using CHANGE to detect both RISING & FALLING edge
  attachInterrupt(digitalPinToInterrupt(interruptPin), log_chg, CHANGE);

  // turn on test LED
  digitalWrite(ledPin, HIGH);
  Serial.println("LED_ON");

  reset_ts();
  
  Serial.println("setup() - nearly end");
  delay(2000);
  digitalWrite(ledPin, LOW);
  Serial.println("LED_OFF");
  Serial.println("setup() - end");
}

void loop() {
  unsigned long curr_us = 0;
  unsigned long quiet_us = 0;
  //unsigned long last_clap_fall_ts = 0;
  //unsigned int clap_cnt = 0;

  //if (arry_idx)
  if (first_rise_ts)
  {
    noInterrupts();
    curr_us = micros();
    if (last_fall_ts)
    {
       quiet_us = curr_us - last_fall_ts;
    }
    
    if (quiet_us > SINGLE_CLAP_SILENT_USEC)
    {
      if (first_rise_ts)
      {
        // do followings ONLY if first_rise_ts exist, ie not just a last_fall_ts detected
/*
        Serial.print("clap width = ");
        Serial.print(last_fall_ts);
        Serial.print(" - ");
        Serial.print(first_rise_ts);
        Serial.print(" = ");
        Serial.println(last_fall_ts - first_rise_ts);
*/
        clap_cnt++;
        last_clap_fall_ts = last_fall_ts;
        digitalWrite(ledPin, HIGH);
      }
      reset_ts();
    }
    interrupts();
  }

  if (clap_cnt)
  {
    delay(50);
    curr_us = micros();
    quiet_us = curr_us - last_clap_fall_ts;
/*
    Serial.print("curr_us = ");
    Serial.println(curr_us);
    Serial.print("last_clap_fall_ts = ");
    Serial.println(last_clap_fall_ts);
    Serial.print("quiet_us = ");
    Serial.println(quiet_us);
    Serial.print("clap count = ");
    Serial.println(clap_cnt);
    Serial.println("-----------------------------------------------");
*/    
    if (quiet_us > ALL_CLAP_SILENT_USEC)
    {
      // print ONLY if first_rise_ts exist, ie not just a last_fall_ts detected
      Serial.print("clap count = ");
      Serial.println(clap_cnt);
      Serial.println();
      digitalWrite(ledPin, LOW);
      clap_cnt = 0;
      last_clap_fall_ts = 0;
    }
  }
}

void log_chg() {
  unsigned long curr_us;
  int curr_val;

  curr_us = micros();
  curr_val = digitalRead(interruptPin);

//  if (first_rise_ts == 0)  <<-- this doesn't work
  if ((first_rise_ts == 0) && (curr_val == HIGH))
  {
     last_fall_ts = first_rise_ts = curr_us;
  }
  if ((curr_us - last_fall_ts) && (curr_val == LOW))
  {
     last_fall_ts = curr_us;
  }
}

void reset_ts() 
{
  first_rise_ts = 0;
  last_fall_ts = 0;
}


