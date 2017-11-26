#include "encoder.h"

unsigned int slot_R = 0;
unsigned int slot_L = 0;
unsigned int r_encoder_count = 0;
unsigned int l_encoder_count = 0;

unsigned l_slot_count=0;
unsigned r_slot_count=0;
unsigned int L_E_count=0;
unsigned int R_E_count=0;
unsigned long sec_count=millis();
int slot_count_time = 1000; //slot counter timer

//
// encoder_update() is 1ms Timer ISR; ie called once a millisecond
//

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
        l_slot_count++; //MY 1 sec count
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
        r_slot_count++; //MY 1 sec count
        r_encoder_count++;
        slot_R=1;
      }
    }
    if ( millis()- slot_count_time > sec_count ) // 1 sec
    {
//      Serial.print(millis()-sec_count); 
      Serial.print(" l_slot_count= ");Serial.print(l_slot_count);
      Serial.print(" r_slot_count= ");Serial.println(r_slot_count);
      L_E_count = l_slot_count ;
      R_E_count = r_slot_count ;
      l_slot_count = r_slot_count = 0; //reset every sec count
      sec_count=millis();

    }
//    else
//    {
//      Serial.println(" N");
//    }
  
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
  return ( l_encoder_count - r_encoder_count );  
}

int get_encoder_1s_count( encoder_Struct *encoder)
{
  encoder->L_E_count_1s = L_E_count;
  encoder->R_E_count_1s = R_E_count;
  return ( L_E_count - R_E_count);
}


