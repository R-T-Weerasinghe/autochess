#include "global.h"
//#include "Micro_Max.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// mux
#define s0 24
#define s1 26
#define s2 28
#define s3 30

#define m0 36
#define m1 38
#define m2 40
#define m3 42

#define MOTOR_BLACK_STEP 29
#define MOTOR_BLACK_DIR 27
#define MOTOR_WHITE_STEP 47
#define MOTOR_WHITE_DIR 45
#define EMAG 53

#define PUSH_BUTTON_BLACK 11
#define PUSH_BUTTON_WHITE 12

#define LIMIT_SWITCH_BLACK 25
#define LIMIT_SWITCH_WHITE 23

#define RESET_PIN 50

#define TROLLEY_START_POSITION_X  4
#define TROLLEY_START_POSITION_Y  7

#define CALLIBRATION_OFFSET 0.1

//*****************************  Additional VARIABLES
bool changeDetected = false;
int gameOverState = 0;  /* 0 - game Ongoing, 1 - first case gameover, 2- Second-case gameover, 3 - third case gameover*/


void gameOver();


//*****************************  EXTERN VARIABLES

//extern byte sequence;
//extern boolean no_valid_move;


char lastM[5];

bool rwarr[2][8];

// mux

bool arr[8][8], arr_backup[8][8];
byte mux_row; // which row the data should be inserted, row 1 or 2 of a mux
byte mux_col;
byte moved_from_letter, moved_from_num, moved_to_letter, moved_to_num; // to record change


LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
   // Reset
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);
  

  Serial.begin(9600);
  Serial2.begin(9600);

  // Mux
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  pinMode(m0, INPUT);
  pinMode(m1, INPUT);
  pinMode(m2, INPUT);
  pinMode(m3, INPUT);

  // elctromagnet

  pinMode(EMAG, OUTPUT);

  // motor
  pinMode(MOTOR_BLACK_STEP, OUTPUT);
  pinMode(MOTOR_BLACK_DIR, OUTPUT);
  pinMode(MOTOR_WHITE_STEP, OUTPUT);
  pinMode(MOTOR_WHITE_DIR, OUTPUT);

  // button
  pinMode(PUSH_BUTTON_BLACK, INPUT);
  pinMode(PUSH_BUTTON_WHITE, INPUT);

  // limits
  pinMode(LIMIT_SWITCH_WHITE, INPUT);
  pinMode(LIMIT_SWITCH_BLACK, INPUT);
  

  delay(1000);
  lcd.init();

  // set hall status for middle ones
  for (byte i = 2; i < 6; i++) {
    for (byte j = 0; j < 8; j++) {
      arr[i][j] = 1;
      arr_backup[i][j] = 1;
    }
  }

  //  MicroMax
  
  timer = millis();
  lcd_display();

  for(byte i=0;i<2;i++){
    for(byte j=0; j<8; j++){
      rwarr[i][j] = 0;
    }
  }
}

void loop() {
  
  switch (sequence) {
    case calibration:
      
      lcd_display();
      Serial.println("callibraing");
      electroMagnet(false);
//      electroMagnet(true);
//      delay(1000);
//      electroMagnet(false);
//      delay(1000);
//      electroMagnet(true);
//      delay(1000);
//      electroMagnet(false);
//      delay(1000);
//      electroMagnet(true);
//      delay(1000);
//      electroMagnet(false);
//      delay(1000);
//       electroMagnet(true);
//      delay(1000);
         
//      electroMagnet(true);
      calibrate();
      sequence = player_white;
      break;

    case player_white:
      electroMagnet(false);
      if (millis() - timer > 1000) {  // Display the white player clock
        countdown();
        lcd_display();
      }
     
       detect_human_movement();
      
     if (digitalRead(PUSH_BUTTON_WHITE)) {
      // Serial.print("am"); 
      // Serial.println(availableMemory()); 
           lcd.setCursor(0, 0);
           lcd.print("     Computer      ");
           lcd.setCursor(4, 1);
           lcd.print(" Thinking ");
        // White player end turn
        new_turn_countdown = true;
        if(changeDetected){
        //  Serial.println("changeDetected");
           changeDetected = false;
           lcd.setCursor(15, 1);
           lcd.print("1");
           player_displacement();

           lcd.setCursor(15, 1);
           lcd.print("2");
           reply_reading();
           lcd.setCursor(15, 1);
           lcd.print("2");
          
           lcd.setCursor(15, 1);
           lcd.print("3");
          
           lcd.setCursor(15, 1);
           lcd.print("4");
        }
        
        
       // print_sensors();  
     }  

      if (digitalRead(PUSH_BUTTON_BLACK)){
        sequence = game_over;
      }
      break;

    case player_black:        
      lcd_display();
      black_player_movement();  //  Move the black chess piece
      sequence = player_white;
      break;

    case stalemate:
     lcd_display();
     delay(6000);
     sequence = game_over;
     break;   

    case no_valid_move:
     lcd_display();
     delay(5000);
     sequence = player_white;
     break; 

    case player_wins:
     lcd_display();
     delay(6000);
     sequence = game_over;
     break; 

    case computer_wins:
     lcd_display();
     delay(6000);
     sequence = game_over;
     break;  

    case game_over:

      lcd_display(); 
      delay(1000);
      if(digitalRead(PUSH_BUTTON_WHITE)) {
        // Restart the game code
        Serial2.println('R');
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("Restarting");
        lcd.setCursor(6, 1);
        lcd.print("Game");
        delay(3000);
        lcd.setCursor(2, 0);
        lcd.print("Reposition the");
        lcd.setCursor(2, 1);
        lcd.print("Chess Pieces");
        delay(5000);        
        digitalWrite(RESET_PIN, LOW);
      } else if(digitalRead(PUSH_BUTTON_BLACK)) {
        sequence =exit_game; 
       }
       break;

    case exit_game:

        lcd_display();        
        break; 
    case punishment:
        lcd_display(); 
        delay(6000);
        sequence = game_over;        
        break;    
      
  }
}

void lcd_display() {
  lcd.backlight();
  switch (sequence) {
    case start_up:
      lcd.setCursor(0, 0);
      lcd.print("   AUTOMATIC    ");
      lcd.setCursor(0, 1);
      lcd.print("   CHESSBOARD   ");
      sequence = calibration;
      delay(4000);
      lcd.clear();
   
    case calibration:
      lcd.setCursor(0, 0);
      lcd.print("  CALIBRATION   ");
      break;
    case player_white:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("     WHITE      ");
      lcd.setCursor(4, 1);
     if(second>=10){
        lcd.print(" " + String(minute) + " : " + String(second) + " ");
      } else{
         lcd.print(" " + String(minute) + " : " +"0"+ String(second) + " ");
      }
      break;
    case player_black:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("     BLACK      ");
      lcd.setCursor(4, 1);
      lcd.print("  Turn ");
      // if(second>=10){
      //   lcd.print(" " + String(minute) + " : " + String(second) + " ");
      // } else{
      //    lcd.print(" " + String(minute) + " : " +"0"+ String(second) + " ");
      // }
      break;

    case no_valid_move:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    NO Valid      ");
      lcd.setCursor(9, 1);
      lcd.print("  Move ");
      delay(5000);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Reverse the");
      lcd.setCursor(10, 1);
      lcd.print("Move");
      break; 

    case stalemate:
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Stalemate");
      break;

    case player_wins:
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Player");
      lcd.setCursor(6, 1);
      lcd.print("Wins");
      break; 

    case computer_wins:
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Computer");
      lcd.setCursor(6, 1);
      lcd.print("Wins");
      break;   

    case game_over:

        lcd.setCursor(0, 0);
        lcd.print("Restart ");
        lcd.setCursor(2, 1);
        lcd.print("Game  ");
        lcd.setCursor(8, 0);
        lcd.print("|");
        lcd.setCursor(8, 1);
        lcd.print("|");
        lcd.setCursor(9, 0);
        lcd.print("  Exit");
        lcd.setCursor(9, 1);
        lcd.print("  Game");
        
        break;

     case exit_game:
        lcd.setCursor(0, 0);
        lcd.print("Switch off from");
        lcd.setCursor(0, 1);
        lcd.print("the Power Button");
        break;     

     case punishment:
        lcd.setCursor(0, 0);
        lcd.print("You have done");
        lcd.setCursor(0, 1);
        lcd.print("an invalid Move");
        delay(5000);
        lcd.print("As a punishment");
        lcd.setCursor(0, 1);
        lcd.print("Game'll end");          
        break;
  }
}

//************************************  CALIBRATE
void calibrate() {
    
  //  Slow displacements up to touch the limit switches
  motor(T_B, SPEED_SLOW, 0.8);
  //motor(T_B, SPEED_SLOW, 0.2);
  // testing
  // end of testing
  while (digitalRead(LIMIT_SWITCH_BLACK) == LOW) motor(R_L, SPEED_SLOW, calibrate_speed);
  while (digitalRead(LIMIT_SWITCH_WHITE) == LOW) motor(B_T, SPEED_SLOW, calibrate_speed);
  delay(500);
  // motor(T_B, SPEED_FAST, 0.725);
  // motor(L_R, SPEED_FAST, 0.3);
   
  // offset
  motor(T_B, SPEED_FAST, 0.19);
  motor(L_R, SPEED_FAST, 0.45);
  

  

  //  Rapid displacements up to the Black start position (e7)
  motorHolding(L_R, 100, TROLLEY_START_POSITION_X+1);
  electroMagnet(false);
  motorHolding(T_B, 100, TROLLEY_START_POSITION_Y-1);
  electroMagnet(false);
  delay(500);
  
  
}

//****************************************  MOTOR
void motor(byte direction, int speed, float distance) {

long step_number = 0;
  //  Direction of the motor rotation
  if (direction == B_T || direction == L_R || direction == LR_BT) digitalWrite(MOTOR_WHITE_DIR, HIGH); // anticlockwise
  else digitalWrite(MOTOR_WHITE_DIR, LOW);
  if (direction == R_L || direction == B_T || direction == RL_BT) digitalWrite(MOTOR_BLACK_DIR, HIGH); // anticlockwise
  else digitalWrite(MOTOR_BLACK_DIR, LOW);

  //Calcul the distance
  if (distance == calibrate_speed) step_number = 70;
  else 
  if (direction == RL_BT || direction == LR_TB || direction == LR_BT || direction == RL_TB) step_number = distance * SQUARE_SIZE * 2.0; //  Add an extra length for the diagonal
  else step_number = distance * SQUARE_SIZE;

  //  Active the motors
  for (long x = 0; x < step_number; x++) {
    if (direction == LR_TB || direction == RL_BT) digitalWrite(MOTOR_WHITE_STEP, LOW); // contolled by black motor
    else digitalWrite(MOTOR_WHITE_STEP, HIGH);
    if (direction == RL_TB || direction == LR_BT) digitalWrite(MOTOR_BLACK_STEP, LOW); // controlled by white motor
    else digitalWrite(MOTOR_BLACK_STEP, HIGH);
    delayMicroseconds(speed);
    digitalWrite(MOTOR_BLACK_STEP, LOW);
    digitalWrite(MOTOR_WHITE_STEP, LOW);
    delayMicroseconds(speed);
  }
}

void motorHolding(byte direction, int speed, float distance){
  
  byte reverseDirArr[] = {1, 0, 3, 2, 5, 4, 7, 6};
  motor(direction, speed, distance);
  motor(direction, SPEED_FAST, CALLIBRATION_OFFSET);
  electroMagnet(false);
  motor(reverseDirArr[direction], SPEED_FAST, CALLIBRATION_OFFSET);
  electroMagnet(true);
}

void countdown() {

  //  Set the time of the current player
  if (new_turn_countdown == true ) {
    new_turn_countdown = false;
    if (sequence == player_white) {
      second = second_white;
      minute = minute_white;
    }
    else if (sequence == player_black) {
      second = second_black;
      minute = minute_black;
    }
  }

  //  Countdown
  timer = millis();
     if (second > 0) {
      second--;

    } else {
      if (minute > 0) {
        minute--;
        second = 59;
      } else {
        // Game over or next player's turn
      }
    }

  //  Record the white player time
  if (sequence == player_white) {
    second_white = second;
    minute_white = minute;
    if(minute_white == 0 && second_white == 1){
       sequence = game_over;

    }
  }
  //  Record the black player time
//  else if (sequence == player_black) {
//    second_black = second;
//    minute_black = minute;
//  }
}

void black_player_movement() {

    electroMagnet(false);
  //  Convert the AI characters in variables
  // byte the ints
  int departure_coord_Y = 9-(lastM[0] - 'a' + 1);
  int departure_coord_X = lastM[1] - '0';
  int arrival_coord_Y = 9-(lastM[2] - 'a' + 1);
  int arrival_coord_X = lastM[3] - '0';
  byte displacement_X = 0;
  byte displacement_Y = 0;
  bool resting_position_found = false;


  if(departure_coord_Y<0 || departure_coord_X<0 || arrival_coord_Y<0 || arrival_coord_X<0){
    Serial.println("No valid move");
    return false;
  }

   

  // handeling white capturing
  
  if (arr[arrival_coord_X - 1][8-arrival_coord_Y] == 0) {

     
      displacement_X = abs(arrival_coord_X - trolley_coordinate_X);
      displacement_Y = abs(arrival_coord_Y - trolley_coordinate_Y);

     //Serial.println("in the white capturing mode :");

       lcd.setCursor(0, 1);
       lcd.print("W");
       lcd.setCursor(1, 1);
       lcd.print(departure_coord_X);
       lcd.setCursor(2, 1);
       lcd.print(departure_coord_Y);

      

    if (arrival_coord_X > trolley_coordinate_X) motor(T_B, SPEED_FAST, displacement_X);
    else if (arrival_coord_X < trolley_coordinate_X) motor(B_T, SPEED_FAST, displacement_X);
    if (arrival_coord_Y > trolley_coordinate_Y) motor(L_R, SPEED_FAST, displacement_Y);
    else if (arrival_coord_Y < trolley_coordinate_Y) motor(R_L, SPEED_FAST, displacement_Y);

      electroMagnet(true);
      delay(500);
      motorHolding(R_L, SPEED_SLOW, 0.5);
      motorHolding(T_B, SPEED_SLOW, 8.5 - arrival_coord_X );
      motorHolding(R_L, SPEED_SLOW, arrival_coord_Y -1);
      
      //Serial.println("it");
       for(byte a=0;a<2;a++){
          for(byte b=0; b<8; b++){
             if(rwarr[a][b]==0){
               //Serial.print(a);
               //Serial.println(b);
               // taking the captured piece to it's resting place
               if(a==1){
                  motorHolding(R_L, SPEED_SLOW, 1);
               }

               motorHolding(B_T, SPEED_SLOW, b+0.5);
               motorHolding(R_L, SPEED_SLOW, 0.4);
               electroMagnet(false);

              

               // retrieving the trolly back to the departure cordindates

                 lcd.setCursor(0, 1);
                 lcd.print("D");
                 lcd.setCursor(1, 1);
                 lcd.print(departure_coord_X);
                 lcd.setCursor(2, 1);
                 lcd.print(departure_coord_Y);
                 
               
               trolley_coordinate_X = 8-b;
               
               
               displacement_X = abs(departure_coord_X - trolley_coordinate_X);
               displacement_Y = abs(departure_coord_Y + a);

               if (departure_coord_X > trolley_coordinate_X) motorHolding(T_B, SPEED_SUPER_FAST, displacement_X);
               else if (departure_coord_X < trolley_coordinate_X) motorHolding(B_T, SPEED_SUPER_FAST, displacement_X);
               electroMagnet(false);
               motorHolding(L_R, SPEED_SUPER_FAST, displacement_Y);
               electroMagnet(false);

               resting_position_found = true;

               rwarr[a][b] = 1;

               break;

               

             }

             

            
          }

           if(resting_position_found){
                resting_position_found = false;
                break;
             }
        }
      //motor(L_R, SPEED_FAST, 0.5);
      //motor(T_B, SPEED_FAST, arrival_coord_X - 0.5);
      trolley_coordinate_X = departure_coord_X;
      trolley_coordinate_Y = departure_coord_Y;

      lcd.setCursor(0, 1);
      lcd.print("T");
      lcd.setCursor(1, 1);
      lcd.print(trolley_coordinate_X);
      lcd.setCursor(2, 1);
      lcd.print(trolley_coordinate_Y);
      delay(300);

  }
    else {
      displacement_X = abs(departure_coord_X - trolley_coordinate_X);
      displacement_Y = abs(departure_coord_Y - trolley_coordinate_Y);

       lcd.setCursor(0, 1);
       lcd.print("D");
       lcd.setCursor(1, 1);
       lcd.print(departure_coord_X);
       lcd.setCursor(2, 1);
       lcd.print(departure_coord_Y);
      
      // Serial.println("trolley_coordinate_X :");
      // Serial.println(trolley_coordinate_X);
      // Serial.println("trolley_coordinate_Y :");
      // Serial.println(trolley_coordinate_Y);
      // Serial.print("displacement_X :");
      // Serial.println(displacement_X);
      // Serial.print("displacement_Y :");
      // Serial.println(displacement_Y);

    if (departure_coord_X > trolley_coordinate_X) motor(T_B, SPEED_FAST, displacement_X);
    else if (departure_coord_X < trolley_coordinate_X) motor(B_T, SPEED_FAST, displacement_X);
    if (departure_coord_Y > trolley_coordinate_Y) motor(L_R, SPEED_FAST, displacement_Y);
    else if (departure_coord_Y < trolley_coordinate_Y) motor(R_L, SPEED_FAST, displacement_Y);

    trolley_coordinate_X = departure_coord_X;
    trolley_coordinate_Y = departure_coord_Y;
    }
    
   
  
  

  //  Move the Black chess piece to the arrival position
  displacement_X = abs(arrival_coord_X - departure_coord_X);
  displacement_Y = abs(arrival_coord_Y - departure_coord_Y);

  trolley_coordinate_X = arrival_coord_X;
  trolley_coordinate_Y = arrival_coord_Y;

   lcd.setCursor(0, 1);
   lcd.print("A");
   lcd.setCursor(1, 1);
   lcd.print(arrival_coord_X);
   lcd.setCursor(2, 1);
   lcd.print(arrival_coord_Y);

  electroMagnet(true);
  delay(500);
  
  //  Knight displacement
  if (displacement_X == 1 && displacement_Y == 2 || displacement_X == 2 && displacement_Y == 1) {
    if (displacement_Y == 2) {
      if (departure_coord_X < arrival_coord_X) {
        motorHolding(T_B, SPEED_SLOW, displacement_X * 0.5);
        if (departure_coord_Y < arrival_coord_Y) motorHolding(L_R, SPEED_SLOW, displacement_Y);
        else motorHolding(R_L, SPEED_SLOW, displacement_Y);
        motorHolding(T_B, SPEED_SLOW, displacement_X * 0.5);
      }
      else if (departure_coord_X > arrival_coord_X) {
        motorHolding(B_T, SPEED_SLOW, displacement_X * 0.5);
        if (departure_coord_Y < arrival_coord_Y) motorHolding(L_R, SPEED_SLOW, displacement_Y);
        else motorHolding(R_L, SPEED_SLOW, displacement_Y);
        motorHolding(B_T, SPEED_SLOW, displacement_X * 0.5);
      }
    }
    else if (displacement_X == 2) {
      if (departure_coord_Y < arrival_coord_Y) {
        motorHolding(L_R, SPEED_SLOW, displacement_Y * 0.5);
        if (departure_coord_X < arrival_coord_X) motorHolding(T_B, SPEED_SLOW, displacement_X);
        else motorHolding(B_T, SPEED_SLOW, displacement_X);
        motorHolding(L_R, SPEED_SLOW, displacement_Y * 0.5);
      }
      else if (departure_coord_Y > arrival_coord_Y) {
        motorHolding(R_L, SPEED_SLOW, displacement_Y * 0.5);
        if (departure_coord_X < arrival_coord_X) motorHolding(T_B, SPEED_SLOW, displacement_X);
        else motorHolding(B_T, SPEED_SLOW, displacement_X);
        motorHolding(R_L, SPEED_SLOW, displacement_Y * 0.5);
      }
    }
  }

    //  Kingside castling
 else if (departure_coord_X == 8 && departure_coord_Y == 4 && arrival_coord_X == 8 && arrival_coord_Y == 2) {  

    lcd.setCursor(0, 0);
    lcd.print("K");
    motorHolding(R_L, SPEED_SLOW, displacement_Y);
     
    electroMagnet(false);
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print("R1");
    motor(R_L, SPEED_SLOW, 1);
    electroMagnet(true);
    lcd.setCursor(0, 0);
    lcd.print("R2");
    motorHolding(T_B, SPEED_SLOW, 0.5);
    motorHolding(L_R, SPEED_FAST, 2);
    motorHolding(B_T, SPEED_SLOW, 0.5);
    delay(4000);
    lcd.setCursor(0, 0);
    lcd.print("R3");
    electroMagnet(false);
    delay(5000);
    trolley_coordinate_X = 8;
    trolley_coordinate_Y = 3;
//    motor(B_T, SPEED_SLOW, 2);
//    electroMagnet(false);
//    delay(5000);
//    motor(T_B, SPEED_FAST, 1);
//    motor(R_L, SPEED_FAST, 0.5);
//    electroMagnet(true);
//    delay(5000);
//    motor(L_R, SPEED_SLOW, 0.5);
  }
else  if (departure_coord_X == 8 && departure_coord_Y == 4 && arrival_coord_X == 8 && arrival_coord_Y == 6) {  //  Queenside castling

    lcd.setCursor(0, 0);
    lcd.print("Q");
    motorHolding(L_R, SPEED_SLOW, displacement_Y);
  
    electroMagnet(false);
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print("R1");
    motor(L_R, SPEED_SLOW, 2);
    electroMagnet(true);
    lcd.setCursor(0, 0);
    lcd.print("R2");
    delay(4000);
    motorHolding(T_B, SPEED_SLOW, 0.5);
    motorHolding(R_L, SPEED_FAST, 3);
    motorHolding(B_T, SPEED_FAST, 0.5);
    lcd.setCursor(0, 0);
    lcd.print("R3");
    electroMagnet(false);
    trolley_coordinate_X = 8;
    trolley_coordinate_Y = 5;
    
  }
  
  //  Diagonal displacement
  else if (displacement_X == displacement_Y) {
    if (departure_coord_X > arrival_coord_X && departure_coord_Y > arrival_coord_Y) motorHolding(RL_BT, SPEED_SLOW, displacement_X);
    else if (departure_coord_X > arrival_coord_X && departure_coord_Y < arrival_coord_Y) motorHolding(LR_BT, SPEED_SLOW, displacement_X);
    else if (departure_coord_X < arrival_coord_X && departure_coord_Y > arrival_coord_Y) motorHolding(RL_TB, SPEED_SLOW, displacement_X);
    else if (departure_coord_X < arrival_coord_X && departure_coord_Y < arrival_coord_Y) motorHolding(LR_TB, SPEED_SLOW, displacement_X);
  }
  
  //  Horizontal displacement
  else if (displacement_Y == 0) {
    if (departure_coord_X > arrival_coord_X) motorHolding(B_T, SPEED_SLOW, displacement_X);
    else if (departure_coord_X < arrival_coord_X) motorHolding(T_B, SPEED_SLOW, displacement_X);
  }
  //  Vertical displacement
  else if (displacement_X == 0) {
    if (departure_coord_Y > arrival_coord_Y) motorHolding(R_L, SPEED_SLOW, displacement_Y);
    else if (departure_coord_Y < arrival_coord_Y) motorHolding(L_R, SPEED_SLOW, displacement_Y);
  }
  

  
  electroMagnet(false);
  delay(500);

  //  Upadte the reed sensors states with the Balck move
  arr[departure_coord_X - 1][8-departure_coord_Y] = 1;
  arr[arrival_coord_X - 1][8-arrival_coord_Y] = 0;
  arr_backup[departure_coord_X - 1][8-departure_coord_Y] = 1;
  arr_backup[arrival_coord_X - 1][8-arrival_coord_Y] = 0;
}

void electroMagnet(boolean state) {
  if (state == true)  {
    digitalWrite(EMAG, HIGH);
    delay(20);
  }
  else  {
    digitalWrite(EMAG, LOW);    
    delay(20);
  }
}
void detect_human_movement() {
  for(byte i=0; i<16; i++){
    digitalWrite(s0, bitRead(i, 0));
    digitalWrite(s1, bitRead(i, 1));
    digitalWrite(s2, bitRead(i, 2));
    digitalWrite(s3, bitRead(i, 3));

    mux_row = i/8;
    mux_col = i%8;

    arr[0+mux_row][mux_col] = digitalRead(m0);
    arr[2+mux_row][mux_col] = digitalRead(m1);
    arr[4+mux_row][mux_col] = digitalRead(m2);
    arr[6+mux_row][mux_col] = digitalRead(m3);    
  }
  delay(500); // prev 500
  reverse_higher_channels();

    detect_change();
    backup();
}

// free RAM check for debugging. SRAM for ATmega328p = 2048Kb.
int availableMemory() {
    // Use 1024 with ATmega168
    int size = 2048;
    byte *buf;
    while ((buf = (byte *) malloc(--size)) == NULL);
        free(buf);
    return size;
}


void reverse_higher_channels() {
  // reverse higher channels (ch8-ch15) to (ch15-ch8) since higher channel connectors are backward
  bool tmp;
  for(byte i=0; i<8; i+=2) { // select row (all rows need to be changed)
    for(byte j=0; j<4; j++) {  // only going halfway is enough to reverse all 
      tmp = arr[i][j];
      arr[i][j] = arr[i][7-j];
      arr[i][7-j] = tmp;
    }
  }
}
void detect_change(void) {
  for(byte i=0; i<8; i++) {
    for(byte j=0; j<8; j++) {
      if(arr[i][j] != arr_backup[i][j]) { // if there is a change
        if(arr[i][j]==1) { // moved_from
          moved_from_letter = j;
          moved_from_num = i;
        } 
        if(arr[i][j]==0 ) {
           if(moved_from_letter!=j || moved_from_num!=i){
              changeDetected = true;
              moved_to_letter = j;
              moved_to_num = i;
           }
          
        }
      }
    }
  }
  // if(did_move_from && did_move_to){
  //   did_move_from = false;
  //   did_move_to = false;
  //   return true;
  // } 
  // else return false;
}
void backup(void) {
  for(byte i=0; i<8; i++) {
    for(byte j=0; j<8; j++) {
      arr_backup[i][j] = arr[i][j];
    }
  }
}
void player_displacement() {
  char num_table[] = {'1', '2', '3', '4', '5', '6', '7', '8'};
  char letter_table[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

  mov[0] = letter_table[moved_from_letter];
  mov[1] = num_table[moved_from_num];
  mov[2] = letter_table[moved_to_letter];
  mov[3] = num_table[moved_to_num];
  
 
  Serial2.print(mov[0]);
  Serial2.print(mov[1]);
  Serial2.print(mov[2]);
  Serial2.print(mov[3]);
  Serial2.print('\n');
  Serial.print("Raw Sending move: ");
  Serial.println(mov);
  Serial.print("Formatted Sending move: ");
      Serial.print(mov[0]);
      Serial.print(mov[1]);
      Serial.print(mov[2]);
      Serial.print(mov[3]);
      Serial.print('\n');
}


void reply_reading(){
  char myData[4];
  while(Serial2.available()==0) {
    //Serial.println("Waiting for node");
  }

  byte n = Serial2.readBytesUntil('\n', myData, 16);
  if(n!=0){
    if(myData[0]=='S'){
       sequence = stalemate;
       Serial.print("reply Status: ");
       Serial.println(myData[0]);
    }else if(myData[0]=='N'){
       sequence = no_valid_move;
       Serial.print("reply Status: ");
       Serial.println(myData[0]);
    }else if(myData[0]=='P'){
       sequence = player_wins;
       Serial.print("reply Status: ");
       Serial.println(myData[0]);    
    }else if(myData[0]=='C'){
       sequence = computer_wins;
       Serial.print("reply Status: ");
       Serial.println(myData[0]); 
     }else if(myData[0]=='I'){
       sequence = punishment;
       Serial.print("reply Status: ");
       Serial.println(myData[0]);
      }
//      else if ('a'<=myData[0]&& myData[0]<='h' )
//    {        
//
//    Serial.print("raw receiving reply move: ");
//    Serial.println(myData);
//    for(int i=0; i<4; i++) {
//      lastM[i] = myData[i];
//    }
//    sequence = player_black;
//
//     Serial.print("formatted reply move: ");
//     Serial.println(lastM);
//  } 
  else{
     int x = 0;
     while(!('a'<=myData[x] && myData[x]<='h')){
         
         x+=1;
         if(x>15){
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("Serial");
            lcd.setCursor(6, 1);
            lcd.print("Error");
            delay(10000);
            break;
          }
      
      }
      
    Serial.print("raw receiving reply move: ");
    Serial.println(myData);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("mydata");
            lcd.setCursor(0, 1);
            lcd.print(myData);
            delay(5000);
    
    for(int i=0; i<4; i++) {
      lastM[i] = myData[i+x];
    }
    sequence = player_black;

    Serial.print("formatted reply move: ");
    Serial.println(lastM);

      
    }
  }


 

}

void print_sensors() {
  Serial.println("--- Board Configuration ---");
  for(byte i=0; i<8; i++) {
    for(byte j=0; j<8; j++) {
      Serial.print(arr[i][j]);
    }
    Serial.println("");
  }  
  Serial.println("----------------------------");
}

// Micromax.cpp

//******************************  INCLUDING FILES
//#include "Arduino.h"
//#include "Micro_Max.h"



//************************************  GAME OVER
void gameOver() {
  sequence = game_over;
}
