/* Drive Motor control in command mode 
 * "mode" of clap_command count of clap collected 
 * speed_R & speed_L motor speed L & R, 
 * FWD_REV motor direction 0 FWD, 1 REV
 */
 void motor_drive(int drive_mode, int speed_R, int speed_L) 
{

switch(drive_mode)
      {
        case 1:
//              Serial.println("    Motor FWD");
              car_dir_flag = 0;
              motor(speed_R,speed_L,car_dir_flag);// car drive FWD
              break;
        case 2:
//              Serial.println("    Motor REV");
              car_dir_flag = 1;
              motor(speed_R,speed_L,1);// car REV
              break;
        case 3:
              Serial.println("    Motor Mode 3");
              break;
        default:
              Serial.println("    Motor unknown");
        break;
      }
}

void motor( int speed_R,int speed_L, int FWD_REV) 
{
  
  digitalWrite(IN1, FWD_REV);  
  digitalWrite(IN4, FWD_REV); 

  //Speed COntrol
 
  analogWrite(IN2,speed_R); //Speed control
  analogWrite(IN3,speed_L);

}
