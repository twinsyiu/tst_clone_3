#ifndef ENCODER_H
#define ENCODER_H
const byte ENC_L_PIN =13;     // 
const byte ENC_R_PIN =12;     // 

void start_encoder_count(unsigned int duration_ms);

typedef struct
{
  unsigned int l_encoder_count;
  unsigned int r_encoder_count;
} encoder_Struct;

void encoder_update();
void reset_encoder_count();
int get_encoder( encoder_Struct *encoder );

#endif


/*
#ifndef RGBLED_HDLR_H
#define RGBLED_HDLR_H
const byte RGB_BLUE_PIN =11;      // bit2
const byte RGB_GREEN_PIN =12;     // bit1
const byte RGB_RED_PIN =13;       // bit0

// colour encode
const byte COLOUR_NIL   = 0b00000000;
const byte COLOUR_RED   = 0b00000001;
const byte COLOUR_GREEN = 0b00000010;
const byte COLOUR_BLUE  = 0b00000100;
const byte COLOUR_WHITE = 0b00000111;

void RGB_LED_init(void);
void RGB_LED_set(byte colour);

byte colour_arry[5] = {COLOUR_NIL, COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE, COLOUR_WHITE};

#endif
*/
