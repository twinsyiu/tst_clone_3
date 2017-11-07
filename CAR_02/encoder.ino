#include "encoder.h"

unsigned int slot_R = 0;
unsigned int slot_L = 0;
unsigned int r_encoder_count = 0;
unsigned int l_encoder_count = 0;

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
        r_encoder_count++;
        slot_R=1;
      }
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
  return ( l_encoder_count - r_encoder_count );
  
}



