//TY: RGB_PIN (active HI to turn on the LED)
//MY: RGB_PIN (active LOW to turn on the LED)

// NOTE : ----------------------VVVVVVVVVV
// please activate the following #include for MY's board
#define MY_ACTIVE_LOW_RGB_LED

#include "RGBLED_Hdlr.h"

#ifdef SERIAL_DBG_ON    
#undef SERIAL_DBG_ON    
#endif

const byte RD_bit = 0;
const byte GN_bit = 1;
const byte BL_bit = 2;

void RGB_LED_init(void) 
{
  pinMode(RGB_BLUE_PIN,OUTPUT);
  pinMode(RGB_GREEN_PIN,OUTPUT);
  pinMode(RGB_RED_PIN,OUTPUT);
}

void RGB_LED_set(byte colour) 
{
#ifdef SERIAL_DBG_ON    
  Serial.print("colour = ");
  Serial.println(colour);
#endif

#ifndef MY_ACTIVE_LOW_RGB_LED
  digitalWrite(RGB_RED_PIN, bitRead(colour, RD_bit)? HIGH : LOW );
  digitalWrite(RGB_GREEN_PIN, bitRead(colour, GN_bit)? HIGH : LOW );
  digitalWrite(RGB_BLUE_PIN, bitRead(colour, BL_bit)? HIGH : LOW );
#else
  digitalWrite(RGB_RED_PIN, bitRead(colour, RD_bit)? LOW : HIGH );
  digitalWrite(RGB_GREEN_PIN, bitRead(colour, GN_bit)? LOW : HIGH );
  digitalWrite(RGB_BLUE_PIN, bitRead(colour, BL_bit)? LOW : HIGH );
#endif
}

