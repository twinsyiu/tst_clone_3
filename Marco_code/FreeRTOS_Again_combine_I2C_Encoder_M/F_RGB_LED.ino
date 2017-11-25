/* RGB LED control with importing "mode" 
 * from clap_command of all clap count collected
 */
void WriteIo(uint8_t bits)
{
  Wire.beginTransmission(address);
  Wire.write(bits);
  Wire.endTransmission();
}

void I2C_RGB_LED( int mode)
{
  uint8_t bits = B11111111; // All bits off
  switch (mode)
  {
    case 1:
          bitClear(bits,4); //Blue LED R
          break;
    case 2:
          bitClear(bits,3); //Green R
          break;
    case 3:         
          bitClear(bits,2); //Red R
          break;
    case 4:
          bitClear(bits,7); //Blue LED L
          bitClear(bits,3); //green LED R
          break;
    case 5:
          bitClear(bits,6); //Green L
          bitClear(bits,2); //red LED R
          break;
    case 6:         
          bitClear(bits,5); //Red L
          bitClear(bits,4); //blue LED R
          break;          
    case 7:
          bitClear(bits,4);//blue
          bitClear(bits,7);//blue
          break;
    case 8:
          bitClear(bits,3);//green
          bitClear(bits,6); //green
          break;
    case 9:
          bitClear(bits,2);//red
          bitClear(bits,5); //red
          break;
    case 10:
    
          bits = B00000000;//all ON
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

