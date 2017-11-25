/* Drive Motor control in command mode 
 * "mode" of clap_command count of clap collected 
 * speed_R & speed_L motor speed L & R, 
 * FWD_REV motor direction 0 FWD, 1 REV
 */
 void motor_drive(int mode ) 
{
  //motor(speed_R,speed_L,DIR_L,DIR_R)
switch(mode)
      {
        case 1:
//              Serial.println("    Motor FWD");
              car_dir_flag = 0;
              motor(set_speed_R,set_speed_L,0,0);// car drive FWD
              break;
        case 2:
//              Serial.println("    Motor REV");
              car_dir_flag = 1;
//              motor(256-set_speed_R,256-set_speed_L,1,0);// car REV
              motor(80,80,1,1);
              break;
        case 3:
              Serial.println("    Motor Mode 3");
              motor(0,0,0,0);
              break;
        default:
              Serial.println("    Motor unknown");
              motor(0,0,0,0);
 
        break;
      }
}

void motor( int speed_R,int speed_L, int FWD_REV_R,int FWD_REV_L) 
{
// Serial.print("Motor started to Drive = ");Serial.print( speed_R);Serial.print(speed_L);
// Serial.println(FWD_REV);
 
 //motor direction with ENA & ENB bypassed on board
  
  digitalWrite(IN1, FWD_REV_R);  
  digitalWrite(IN4, FWD_REV_L); 

  //Speed COntrol
 
  analogWrite(IN2,speed_R); //Speed control
  analogWrite(IN3,speed_L);

  

 
 /*
 // this function will run the motors in both directions at a fixed speed
  // turn on motor A
  digitalWrite(in1, L1);
  digitalWrite(in2, L2);
  // set speed to enA_L out of possible range 0~255
  analogWrite(enA, enA_L);
  // turn on motor B
  digitalWrite(in3, R1);
  digitalWrite(in4, R2);
  // set speed to enB_R out of possible range 0~255
  analogWrite(enB, enB_R); 
  */  
}
