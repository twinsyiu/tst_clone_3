#include "encoder.h"

unsigned int slot_R = 0;
unsigned int slot_L = 0;
unsigned int r_encoder_count = 0;
unsigned int l_encoder_count = 0;

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

int get_encoder( encoder_Struct *encoder )
{
  encoder->l_encoder_count = l_encoder_count;
  encoder->r_encoder_count = r_encoder_count;
  if ( l_encoder_count == r_encoder_count )
  {
    return 0;
  }
  else if ( l_encoder_count > r_encoder_count )
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

void start_encoder_count(unsigned int duration_ms)
{
/*
  unsigned int start_ts;
  unsigned int current_ts;
  
  int slot_L = 0;
  int slot_R = 0;

  unsigned int l_encoder_count = 0;
  unsigned int r_encoder_count = 0;

  start_ts = millis();

  while ((millis() - start_ts) < duration_ms)
  {
    // LEFT ENCODER COUNTING ---------------------
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

    delay(10);    
  }
  current_ts = millis();

  Serial.print("l_encoder_count : ");
  Serial.println(l_encoder_count);
  Serial.print("r_encoder_count : ");
  Serial.println(r_encoder_count);

  Serial.print("current_ts : ");
  Serial.println(current_ts);
  Serial.print("start_ts : ");
  Serial.println(start_ts);
  Serial.print("actual duration : ");
  Serial.println(current_ts - start_ts);
*/
}



void stop_encoder_count()
{
  // l_encoder_count = 0;
  // r_encoder_count = 0;
}


