#include <Adafruit_NeoPixel.h>

// Constants and pin definitions
#define NUMPIXELS 12
#define neo_led 6
#define PIN A1

char game_board[3][3];
int game_mode_button = 4;
int player_turn_led = 9;
int game_mode_led = 10;
int reset_button = 3;
int neo_pixel_index;
char winner_val;
int current_player = LOW;
int button_values[9] = { 90, 180, 335, 511, 612, 746, 815, 910, 1000 };  //[]
int count;
bool button_pressed = false;
void singlePlayerGame(int value);
void multiPlayerGame(int value);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, neo_led, NEO_GRB + NEO_KHZ800);





void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.show();

  pinMode(neo_led, OUTPUT);
  pinMode(PIN, INPUT);
  pinMode(reset_button, INPUT_PULLUP);
  pinMode(game_mode_button, INPUT_PULLUP);


  randomSeed(analogRead(A0));


  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {

      game_board[i][j] = '-';
    }
  }
}




void loop() {
  int read_value = analogRead(PIN);
  //Serial.println(read_value);
  int game_mode = digitalRead(game_mode_button);






  //the defualt game mode is single player with ai

  if (game_mode == HIGH) {
    //checkResults();
    multiPlayerGame(read_value);

  } else if (game_mode != HIGH) {
    //checkResults();
    singlePlayerGame(read_value);
  }


  if (reset_button == LOW) {
    resetGame();
  }
}







void multiPlayerGame(int value) {
  
  int count = 0;
  if (current_player == HIGH) {
    for (int i = 0; i < 3; i++) {

      for (int j = 0; j < 3; j++) {

        if (game_board[i][j] == '-' && value >= button_values[i * 3 + j] - 0 && value <= button_values[i * 3 + j] + 10) {

          game_board[i][j] = 'O';
          count++;
          Serial.println(value);

          updateLeds(i, j, current_player);
          current_player = LOW;
         
        }
      }
    }

  }




  else if (current_player == LOW) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {

        if (game_board[i][j] == '-' && value >= button_values[i * 3 + j] - 10 && value <= button_values[i * 3 + j] + 10) {

          game_board[i][j] = 'X';


          updateLeds(i, j, current_player);
          current_player = HIGH;
          
        }
      }
    }
    
  }
}








void singlePlayerGame(int value /*, int player*/) {

  if (current_player == LOW) {
    for (int i = 0; i < 3; i++) {

      for (int j = 0; j < 3; j++) {

        if (game_board[i][j] == '-' && value >= button_values[i * 3 + j] - 10 && value <= button_values[i * 3 + j] + 10) {

          game_board[i][j] = 'X';
          updateLeds(i, j, current_player);
          Serial.println("person playing");
          Serial.print("read value  ");
          Serial.println(value);
          current_player = HIGH;
        }
      }
    }
  } else if (current_player == HIGH) {
    handleAiMove(current_player);
    current_player = LOW;
  }
}







void handleAiMove(int player) {
  int bestScore = -1000;
  int bestMove = -1;


  Serial.println("we have ai");
  Serial.println(count);
 

  for (int i = 0; i < 9; i++) {
    int row = i / 3;
    int col = i % 3;

    if (game_board[row][col] == '-') {
      game_board[row][col] = 'O';

      int score = minimax(game_board, 7, false);

      game_board[row][col] = '-';
 
      if (score > bestScore) {
        bestScore = score;
        bestMove = i;
 
      }
    }
  }

  if (bestMove != -1) {
    int row = bestMove / 3;
    int col = bestMove % 3;
    game_board[row][col] = 'O';
    updateLeds(row, col, player);
  }
}








int evaluateBoard(char game_board[3][3]) {
  // Check rows for a win
  for (int i = 0; i < 3; i++) {
    if (game_board[i][0] == 'O' && game_board[i][1] == 'O' && game_board[i][2] == 'O') {
      return 10;  // Return a high score for AI winning
    }
    // Check 'X' wins similarly

     if (game_board[i][0] == 'X' && game_board[i][1] == 'X' && game_board[i][2] == 'X') {
      return -10; // Return a high negative score for opponent winning
    }
  }

  // Check columns for a win
  for (int i = 0; i < 3; i++) {
    if (game_board[0][i] == 'O' && game_board[1][i] == 'O' && game_board[2][i] == 'O') {
      return 10;  // Return a high score for AI winning
    }

    if (game_board[0][i] == 'X' && game_board[1][i] == 'X' && game_board[2][i] == 'X') {
      return -10; 
    }
   
  }

  // Check diagonals for a win
  if ((game_board[0][0] == 'O' && game_board[1][1] == 'O' && game_board[2][2] == 'O') ||
      (game_board[0][2] == 'O' && game_board[1][1] == 'O' && game_board[2][0] == 'O')) {
    return 10;  
  }

  if ((game_board[0][0] == 'X' && game_board[1][1] == 'X' && game_board[2][2] == 'X') ||
      (game_board[0][2] == 'X' && game_board[1][1] == 'X' && game_board[2][0] == 'X')) {
    return -10; 
 

  return 0; 
}




int minimax(char game_board[3][3], int depth, bool isMaximizing) {
  char result = checkResults(); 

  if (result != '-') {
    return evaluateBoard(game_board);
  }

  
  if (isMaximizing) {
    int bestScore = -1000;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (game_board[i][j] == ' ') {
          game_board[i][j] = 'O'; // AI's move ('O')
          int score = minimax(game_board, depth + 1, false);
          game_board[i][j] = ' '; // Undo the move

          bestScore = max(score, bestScore);
        }
      }
    }
    return bestScore;
  } else {
    int bestScore = 1000;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (game_board[i][j] == ' ') {
          game_board[i][j] = 'X'; // Opponent's move ('X')
          int score = minimax(game_board, depth + 1, true);
          game_board[i][j] = ' '; // Undo the move

          bestScore = min(score, bestScore);
        }
      }
    }
    return bestScore;
  }
}



char checkResults() {
  // Check rows for a win
  for (int i = 0; i < 3; i++) {
    if (game_board[i][0] == game_board[i][1] && game_board[i][1] == game_board[i][2] && game_board[i][0] != '-') {
      return game_board[i][0];  // Return the winning player symbol
    }
  }

  // Check columns for a win
  for (int i = 0; i < 3; i++) {
    if (game_board[0][i] == game_board[1][i] && game_board[1][i] == game_board[2][i] && game_board[0][i] != '-') {
      return game_board[0][i];  // Return the winning player symbol
    }
  }

  // Check diagonals for a win
  if ((game_board[0][0] == game_board[1][1] && game_board[1][1] == game_board[2][2]) || (game_board[0][2] == game_board[1][1] && game_board[1][1] == game_board[2][0])) {
    if (game_board[1][1] != '-') {
      return game_board[1][1];  // Return the winning player symbol
    }
  }

  // Check for a draw
  bool draw ;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (game_board[i][j] == '-') {
        draw = false; // If any cell is empty, the game is not a draw
        break;
      }
    }
  }
  if (draw) {
    return 'd';  // Return 'd' indicating a draw
  }

  // If no winner or draw yet, return '-'
  return '-';
}


void updateLeds(int row, int column, int player) {
  neo_pixel_index = row * 3 + column;

  // Set LEDs based on current player's turn
  if (player == LOW) {
    strip.setPixelColor(player_turn_led, 0, 160, 255);  // Blue for player X turn
    strip.setPixelColor(neo_pixel_index, 0, 160, 255);  // Blue for selected position
    Serial.println("Player X's turn");
  } else if (player == HIGH) {
    strip.setPixelColor(player_turn_led, 255, 0, 100);  // Pink for player O turn
    strip.setPixelColor(neo_pixel_index, 255, 0, 100);  // Pink for selected position
    Serial.println("Player O's turn");
  }
  strip.show();

  char result = checkResults(); 
  if (result == 'X') {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, 0, 255, 0);
    }
    Serial.println("Player X wins!");
  } else if (result == 'O') {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, 255, 0, 0);  
    }
    Serial.println("Player O wins!");
  } else if (result == 'd') {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, 255, 255, 0);  
    }
    Serial.println("It's a draw!");
  }
  strip.show();
}

    void resetGame() {
      // Clear the game game_board
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          game_board[i][j] = ' ';
        }
      }

     
      current_player = LOW;

      
      for (int i = 0; i < NUMPIXELS; i++) {
        strip.setPixelColor(i, 0, 0, 0); 
      }
      strip.setPixelColor(player_turn_led, 0, 0, 0);  
      strip.show();
    }
