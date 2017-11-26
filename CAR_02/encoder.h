#ifndef ENCODER_H
#define ENCODER_H
const byte ENC_L_PIN =13;     // 
const byte ENC_R_PIN =12;     // 

void start_encoder_count(unsigned int duration_ms);

typedef struct
{
  unsigned int l_encoder_count;
  unsigned int r_encoder_count;
  unsigned int L_E_count_1s;
  unsigned int R_E_count_1s;
  unsigned int encoder_ts;
} encoder_Struct;

void encoder_update();
void reset_encoder_count();
int get_encoder( encoder_Struct *encoder );

#endif

