#include "encoder.h"

unsigned int slot_R = 0;      // a flag to remember the state of Right Slot
unsigned int slot_L = 0;      // a flag to remember the state of Left Slot
unsigned int r_encoder_count = 0;
unsigned int l_encoder_count = 0;

const int ONE_SECOND = 1000;
unsigned int ms_counter = 0;

unsigned int l_slot_count=0;
unsigned int r_slot_count=0;
unsigned int L_E_count_1s=0;
unsigned int R_E_count_1s=0;

/*
 * encoder_update() is 1ms Timer ISR; ie it is called on every millisecond
 */
void encoder_update()
{
  if ( slot_L )   // slot_L is 1 ( ie not 0 )
  {
    if (digitalRead(ENC_L_PIN)==0)
    {
      slot_L=0;
    }
  }
  else  // slot_L is 0
  {
    if (digitalRead(ENC_L_PIN))
    {
      l_slot_count++;       // increment the per second left-slot counter
      l_encoder_count++;
      slot_L=1;
    }
  }

  // RIGHT ENCODER COUNTING ---------------------
  if ( slot_R )   // slot_R is 1 ( ie not 0 )
  {
    if (digitalRead(ENC_R_PIN)==0)
    {
      slot_R=0;
    }
  }
  else  // slot_R is 0
  {
    if (digitalRead(ENC_R_PIN))
    {
      r_slot_count++;       // increment the per second right-slot counter
      r_encoder_count++;
      slot_R=1;
    }
  }

  ms_counter++;
  if (ms_counter >= ONE_SECOND)
  {
    ms_counter = 0;
//    Serial.print(" l_slot_count= ");Serial.print(l_slot_count);
//    Serial.print(" r_slot_count= ");Serial.println(r_slot_count);
    L_E_count_1s = l_slot_count ; // capture the per second left-slot counter
    R_E_count_1s = r_slot_count ; // capture the per second right-slot counter
    
    l_slot_count = r_slot_count = 0; // clear both per second counters
    
  }
}

void reset_encoder_count()
{
  l_encoder_count = 0;
  r_encoder_count = 0;
}

//
// get_encoder() will put both encoder counter value to the caller supplied struct encoder;
// it returns the diff between l and r encoder; ie l_encoder_count - r_encoder_count
//
int get_encoder( encoder_Struct *encoder )
{
  encoder->l_encoder_count = l_encoder_count;
  encoder->r_encoder_count = r_encoder_count;
  encoder->L_E_count_1s = L_E_count_1s;
  encoder->R_E_count_1s = R_E_count_1s;

  encoder->encoder_ts = millis();
  return ( l_encoder_count - r_encoder_count );
  
}



