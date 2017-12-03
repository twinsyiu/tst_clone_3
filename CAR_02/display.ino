const int LCD_update_period_ms = 250;   // the LCD will be updated every 250ms
unsigned long next_disp_time_ms;

/*
 * dislplay_to_LCD() will display the corresponding sign of the movement state onto the LCD,
 * the three ultrasound sensors measured distance to the obstacles;
 * and the PWM (%) used on left and right motor
 *
 * this function will be activated every 250ms, ie the LCD will be updated every 250ms
 */
void dislplay_to_LCD( void )
{

  unsigned long current_time;

  current_time = millis();                // the millis() will role-over in 63 days (TODO if battery can last that long
  if ( current_time > next_disp_time_ms )
  {
    next_disp_time_ms = current_time + LCD_update_period_ms;
//    Serial.print(current_time);Serial.print(" --lcd ");Serial.println(next_disp_time_ms);
    switch (movement_state)
    {
      case MVSTATE_REV:
        display_dist("  V  ");
        break;
      case MVSTATE_STOP:
        display_dist("  0  ");
        break;
      case MVSTATE_GO_FWD:
        display_dist("  ^  ");
        break;
      case MVSTATE_KEEP_LEFT:
        display_dist("  |> ");
        break;
      case MVSTATE_KEEP_RIGHT:
        display_dist(" <|  ");
        break;
      case MVSTATE_TURN_LEFT:
        display_dist("<<<  ");
        break;
      case MVSTATE_TURN_RIGHT:
        display_dist("  >>>");
        break;
      case MVSTATE_DEAD_STOP:
        display_dist("  -  ");
        break;
      case MVSTATE_PAUSE:
        display_dist("PAUSE");
        break;
      default:
        display_dist("@@@@@");
        break;
    }
  }
}

/*
 * lcd_space_print() fills the empty spaces required after the ultrasound sensors measured distance to obstacles
 */
void lcd_space_print (int dist)
{
  if (dist < 10)
  {
    lcd.print("  ");
    return;
  }
  if (dist < 100)
    lcd.print(" ");
}

/*
 * display_dist() will display the parameters to the predefined position
 */
void display_dist (const char * dir_str)
{
  lcd.setCursor(0,0); // set the cursor to column 0, line 0
  lcd.print("L");
  lcd.setCursor(1,0); // set the cursor to column 1, line 0
  lcd.print(l_dist); // Print l_dist to the LCD.
  lcd_space_print (l_dist);

  lcd.setCursor(6,0); // set the cursor to column 6, line 0
  lcd.print("F");
  lcd.setCursor(7,0); // set the cursor to column 7, line 0
  lcd.print(f_dist); // Print l_dist to the LCD.
  lcd_space_print (f_dist);

  lcd.setCursor(12,0); // set the cursor to column 12, line 0
  lcd.print("R");
  lcd.setCursor(13,0); // set the cursor to column 13, line 0
  lcd.print(r_dist); // Print l_dist to the LCD.
  lcd_space_print (r_dist);

  lcd.setCursor(0,1); // set the cursor to column 0, line 1
  lcd.print(dir_str); // Print input string to the LCD.

  lcd.setCursor(7,1); // set the cursor to column 7, line 1
  lcd.print("LM"); // Print < to the LCD.
  lcd.setCursor(9,1); // set the cursor to column 9, line 1
  lcd.print(l_motor_PWM); // Print < to the LCD.
  lcd.setCursor(12,1); // set the cursor to column 12, line 1
  lcd.print("RM"); // Print < to the LCD.
  lcd.setCursor(14,1); // set the cursor to column 14, line 1
  lcd.print(r_motor_PWM); // Print < to the LCD.

}

/*
 * display_battery() will display the battery voltage at predefined position on the LCD
 */
void display_battery (float vcc_volt)
{
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight

  lcd.setCursor(0,0); // set the cursor to column 0, line 0
  lcd.print("INIT BATTERY TEST"); // Print l_dist to the LCD.

  lcd.setCursor(0,1); // set the cursor to column 0, line 1
  lcd.print(vcc_volt); // Print l_dist to the LCD.

  lcd.setCursor(6,1); // set the cursor to column 6, line 1
  lcd.print("Volt"); // Print l_dist to the LCD.
}


