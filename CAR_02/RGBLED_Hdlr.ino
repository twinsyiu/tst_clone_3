
uint8_t exp_board_address;


void Init_RGBLED ( void )
{
  //I2C
  //  Wire.begin(); // Arduino needs (SDA=A4,SCL=A5)
  exp_board_address = 0x20;    // I2C address of the IO-Expension board
  I2C_RGB_LED(ALL_OFF);
}

/* RGB LED control with importing "mode"
 * from clap_command of all clap count collected
 */
void WriteIo ( uint8_t bits )
{
  Wire.beginTransmission(exp_board_address);
  Wire.write(bits);
  Wire.endTransmission();
}

void I2C_RGB_LED ( int mode )
{
  uint8_t bits = B11111111; // All bits off
  switch (mode)
  {
    case R_RD:
          bitClear(bits,2); //Red R
          break;
    case R_GN:
          bitClear(bits,3); //Green R
          break;
    case R_BL:
          bitClear(bits,4); //Blue LED R
          break;
    case R_WH:
          bits = B11100011; //White R
          break;
    case L_RD_R_RD:
          bitClear(bits,2); //red R
          bitClear(bits,5); //red L
          break;
    case L_RD_R_GN:
          bitClear(bits,5); //Red L
          bitClear(bits,3); //Green R
          break;
    case L_RD_R_BL:
          bitClear(bits,5); //Red L
          bitClear(bits,4); //White R
          break;
    case L_RD_R_WH:
          bits = B11100011; //Blue LED R
          bitClear(bits,5); //Red L
          break;
    case L_GN_R_GN:
          bitClear(bits,3); //green R
          bitClear(bits,6); //green L
          break;
    case L_GN_R_BL:
          bitClear(bits,3); //green R
          bitClear(bits,4); //blue LED R
          break;
    case L_BL_R_BL:
          bitClear(bits,4); //blue R
          bitClear(bits,7); //blue L
          break;
    case L_RD:
          bitClear(bits,5); //Red R
          break;
    case L_GN:
          bitClear(bits,6); //Green R
          break;
    case L_BL:
          bitClear(bits,7); //Blue LED R
          break;

    case L_BL_R_GN:
          bitClear(bits,7); //Blue LED L
          bitClear(bits,3); //green LED R
          break;
    case L_GN_R_RD:
          bitClear(bits,6); //Green L
          bitClear(bits,2); //red LED R
          break;
    case L_WH_R_WH:
          bits = B00000000;//all ON
          break;
    case ALL_OFF:
          bits = B11111111;//all OFF
          break;
  }
  WriteIo(bits);
}

