
uint8_t exp_board_address;


void Init_RGBLED ( void )
{
  //I2C
//  Wire.begin(); // Arduino needs (SDA=A4,SCL=A5)
  exp_board_address = 0x20;    // I2C address of the IO-Expension board
  uint8_t bits = B11111111; // All bits off
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
    case R_BL:
          bitClear(bits,4); //Blue LED R
          break;
    case R_GN:
          bitClear(bits,3); //Green R
          break;
    case R_RD:         
          bitClear(bits,2); //Red R
          break;
    case L_BL:
          bitClear(bits,7); //Blue LED R
          break;
    case L_GN:
          bitClear(bits,6); //Green R
          break;
    case L_RD:         
          bitClear(bits,5); //Red R
          break;
    case L_BL_R_GN:
          bitClear(bits,7); //Blue LED L
          bitClear(bits,3); //green LED R
          break;
    case L_GN_R_RD:
          bitClear(bits,6); //Green L
          bitClear(bits,2); //red LED R
          break;
    case L_RD_R_BL:         
          bitClear(bits,5); //Red L
          bitClear(bits,4); //blue LED R
          break;          
    case L_BL_R_BL:
          bitClear(bits,4); //blue R
          bitClear(bits,7); //blue L
          break;
    case L_GN_R_GN:
          bitClear(bits,3); //green R
          bitClear(bits,6); //green L
          break;
    case L_RD_R_RD:
          bitClear(bits,2); //red R
          bitClear(bits,5); //red L
          break;
    case L_WH_R_WH:
          bits = B00000000;//all ON
          break;      
    case ALL_OFF:
          bits = B11111111;//all ON
          break;      
  }
  WriteIo(bits);
}


void flash_RGB(int ON_LED, int flash_Number) // ON_LED for which LED, flash_Number for how many flash
{
  uint8_t bits = B11111111; // All bits off
  for (int i=0;i<flash_Number;i++)
  {
    I2C_RGB_LED(ON_LED);
    vTaskDelay(25);
    I2C_RGB_LED(0);
    WriteIo(bits);
    vTaskDelay(25);
  }
}

