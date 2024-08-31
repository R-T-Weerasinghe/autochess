//  Chessboard
//const float TROLLEY_START_POSITION_X = 4;
//const float TROLLEY_START_POSITION_Y = 7;
byte trolley_coordinate_X = 7;
byte trolley_coordinate_Y = 4;
char mov [4] = {0, 0, 0, 0};
//boolean no_valid_move = false;
byte reed_colone[2];
byte reed_line[2];

//  Game setting
enum {start_up, start, calibration, player_white, player_black, game_over, exit_game, stalemate, player_wins, computer_wins, no_valid_move, punishment};
byte sequence = start_up;
enum {T_B, B_T, L_R, R_L, LR_BT, RL_TB, LR_TB, RL_BT, calibrate_speed};
// T=Top  -  B=Bottom  -  L=Left  -  R=Right
byte game_mode = 0;
enum {HvsH, HvsC};

//  Electromagnet
const byte MAGNET (6);

//  Countdown
byte second = 60;
byte minute = 9;
byte second_white = second;
byte second_black = second;
byte minute_white = minute;
byte minute_black = minute;
unsigned long timer = 0;
boolean start_black = true;
boolean new_turn_countdown = false;

//  Motor
const byte MOTOR_WHITE_DIR (2);
const byte MOTOR_WHITE_STEP (3);
const byte MOTOR_BLACK_DIR (4);
const byte MOTOR_BLACK_STEP (5);
//const int SQUARE_SIZE = 7250;
const int SQUARE_SIZE = 7350;
const int SPEED_SLOW (100);
const int SPEED_FAST (100);
const int SPEED_SUPER_FAST (20);

//  Multiplexer


//  Button - Switch

enum {WHITE, BLACK};

//  MicroMax
//extern char lastH[], lastM[];
