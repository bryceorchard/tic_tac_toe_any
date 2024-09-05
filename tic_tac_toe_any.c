////////////////////////////////////////////////////////////////////////////////
// Tic tac toe board of any width/height less than 9.
// Change the W and H constants for the dimensions of the board
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Constants

#define W 9
#define H 9
#define INPUT_SIZE 10
#define EXIT -1
#define CONTINUE -2
#define TIE -3
#define AGAIN -4
#define COMPUTER -5
#define PLAYER -6

// Integer return types

/*Integers from 0 - 8, representing board positions*/
typedef int position;

/*Integers from -4 to -1, representing the state of the game*/
typedef int gameState;

/*Integers from -6 to -1, representing the state of the game or a winning player*/
typedef int gameEvent;


// Function prototypes

int (*_CopyArray(int board[H][W]))[H];
int *_GetValidSpots(int board[H][W], int *array_length);
bool IsIn(int value, int array[], int array_length);

position BotAi(int board[H][W]);
gameState Bot(int board[H][W]);

gameEvent CheckHorizontal(int row[W]);
gameEvent CheckVertical(int board[H][W], int index);
gameEvent CheckDiagonals(int board[H][W]);
gameEvent CheckGameOver(int board[H][W], int spots_length);

gameState GameOver(gameEvent winner);
gameState PlayAgain();

void Spacer();
void NumRow(int row[W]);
void Divider();
void DisplayBoard(int board[H][W]);

int ValidNumber(int num_read_success, int count, double num_input);
int GetInput();
void PlayPiece(int board[H][W], position p, int piece);
gameState Player(int board[H][W]);

// Of note, an underscore before a function or variable signifies that it either
// returns a pointer, or is a pointer.


/* Duplicates a 2d array */
/* Returns a pointer to the array of rows of the copied board*/
int (*_CopyArray(int board[H][W]))[H] {
    static int array_copy[H][W];
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            memcpy(&array_copy[i][j], &board[i][j], sizeof(array_copy[i][j]));
        }
    }
    return array_copy;
}

/* Return a pointer to an array of all the valid spots, and sets *_array_length to the amount of valid spots. */
int *_GetValidSpots(int board[H][W], int *_array_length) {
    // When passed to this function, _array_length is unitialized
    static int valid_spots[INPUT_SIZE];
    int counter = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (board[i][j] >= 0 && board[i][j] < (H*W)) {
                valid_spots[counter++] = board[i][j];
            }
        }
    }
    *_array_length = counter;
    return valid_spots;
}

/* Checks if an int exists in an array of ints */
bool IsIn(int value, int array[], int array_length) {
    for (int i = 0; i < array_length; i++) {
        if (array[i] == value) {
            return true;
        } 
    } return false;
}


/* Returns the position of a fairly decent spot where the computer will play */
position BotAi(int board[H][W]) {
    if (board[(H-1)/2][(W-1)/2] >= 0)
        // Play in the middle if possible
        return board[(H-1)/2][(W-1)/2];

    int spots_length;
    int *_valid_spots = _GetValidSpots(board, &spots_length);

    // This first checks all possible positions where the computer can win,
    // once it finds one, it will return that position.
    // If none are found, it then checks for all possible positions where the player can win,
    // and returns the first instance found.
    for (int piece = COMPUTER; piece >= PLAYER; piece--) {
        // piece = -5, then piece = -6
        for (int j = 0; j < spots_length; j++) {
            int (*board2)[H] = _CopyArray(board);
            PlayPiece(board2, _valid_spots[j], piece);

            gameEvent winner = CheckGameOver(board2, spots_length-1);
            if (winner == piece) {
                return _valid_spots[j];
            } 
        }
    }
    // If none exist, pick a random available spot
    srand(time(NULL));
    return _valid_spots[rand() % spots_length];
}

/* Plays the best move on the board, then checks if the computer has won or not. */
gameState Bot(int board[H][W]) {
    position best_move = BotAi(board);
    PlayPiece(board, best_move, COMPUTER);
    
    int spots_length;
    _GetValidSpots(board, &spots_length);
    // spots_length is updated, and we disregard the pointer returned by _GetValidSpots()

    gameEvent result = CheckGameOver(board, spots_length);
    if (result == CONTINUE) {
        return Player(board);
    } else {
        DisplayBoard(board);
        return GameOver(result);
    }
}

gameEvent CheckHorizontal(int row[W]) {
    for (int j = 0; j < W-1; j++) {
            if (row[j] != row[j+1]) {
                return CONTINUE;
            }
        }
    return row[0];
}

gameEvent CheckVertical(int board[H][W], int index) {
    for (int j = 0; j < H-1; j++) {
            if (board[j][index] != board[j+1][index]) {
                return CONTINUE;
            }
        }
    return board[0][index];
}

gameEvent CheckDiagonals(int board[H][W]) {
    for (int j = 0; j < H-1; j++) {
        if (board[j][j] != board[j+1][j+1]) {
            break;
        } else if (j == H - 2)
            return board[0][0]; 
    }

    for (int i = 0, j = H-1; i < H-1 && j >= 0; i++, j--) {
        if (board[i][j] != board[i+1][j-1]) {
            break;
        } else if (i == H - 2)
            return board[0][W-1]; 
    }
    return CONTINUE;
}

/* Return the winning player, otherwise return -2 if the game is not over. */
/* Return -3 if it's a tie. */
gameEvent CheckGameOver(int board[H][W], int spots_length) {
    for (int i = 0; i < H; i++) {
        // Check horizontals
        int horizontal = CheckHorizontal(board[i]);
        if (horizontal != CONTINUE) {
            return horizontal;
        }
        // Check verticals
        int vertical = CheckVertical(board, i);
        if (vertical != CONTINUE) {
            return vertical;
        }
    }

    int diagonal = CheckDiagonals(board);
    if (diagonal != CONTINUE) {
        return diagonal;
    }

    if (spots_length > 0) {
        // If there are no three in a row, and there are still valid spots left
        return CONTINUE;
    }
    return TIE; // Only other possibility
}

gameState GameOver(gameEvent result) {
    switch (result) {
        case COMPUTER:
            printf("You lose:(\n");
            break;
        case PLAYER:
            printf("You win!\n");
            break;
        case TIE:
            printf("It's a tie!\n");
    }
    printf("Would you like to play again? (y/n)\n");
    return PlayAgain();
}

gameState PlayAgain() {
    char input[INPUT_SIZE];
    int temp, count = 0;

    // This approach reads input character by character, allowing for precise 
    // control over input handling, including boundary conditions and buffer 
    // overflow prevention, which is safer and more robust than using scanf("%s").
    while ((temp = getchar()) != ' ' && temp != '\n' && temp != EOF && count < INPUT_SIZE) {
        input[count++] = temp;
    } input[count] = '\0'; // Must add terminating character

    if (strcmp(input, "y") == 0) {
        return AGAIN;
    } else if (strcmp(input, "n") == 0) {
        return EXIT;
    } 
    printf("Type either 'y' or 'n' to continue.\n");
    return PlayAgain();
}

// Prints the board
void Spacer() {
    for (int i = 0; i < W; i++) {
        printf("      ");
        if (i != W - 1) 
            printf("|");
    } printf("\n");
}

void NumRow(int row[W]) {
    for (int k = 0; k < W; k++) {
        if (row[k] >= 0) {
            printf("  %02d  ", row[k]);
        } else if (row[k] == COMPUTER) {
            printf("   O  ");
        } else if (row[k] == PLAYER)
            printf("   X  ");

        if (k != W-1)
            printf("|");
    } printf("\n");
}

void Divider() {
    for (int l = 0; l <= (7*W - 1); l++) {
        printf("-");
    } printf("\n");
}

void DisplayBoard(int board[H][W]) {
    for (int i = 0; i < H; i++) {
        Spacer();
        NumRow(board[i]);
        Spacer();
        if (i != W-1)
            Divider();    
    }
}

/* Evaluates whether a number in GetInput() is a possible position on the board. */
/* 1 for true, 0 for false, and -1 if no number exists in the input. */
int ValidNumber(int num_read_success, int num_chars, double num_input) {
    if (num_read_success != 0 && num_chars > 0) {
        printf("Invalid input.\n");
        return 0;

    } else if (num_input < (double)-(W*H) || num_input > (double)(W*H)) {
        printf("Input is too large.\n");
        return 0;

    } else if (fabs(round(num_input) - num_input) > 0.00001) {
        printf("Only integers are allowed.\n");
        return 0;

    } else if (num_input < 0.0 || num_input > (H*W) - 1.0) {
        printf("Spot '%.0lf' does not exist.\n", num_input);
        return 0;

    } else if (num_chars == 0) {
        // If no string has been detected in the input and the number is valid
        return 1;
    } return -1;
}

/* Return input if it's from 0 to H*W, otherwise return -1 if input is 'exit', or -4 if the input is 'restart' */
int GetInput() {
    double num_input;
    char char_input[INPUT_SIZE];
    int temp, num_chars = 0;

    // This approach is very similar to PlayAgain(), however, since both integers and strings are being read,
    // there are considerably more edge cases to consider. example:
    // stdin = '1.0a'
    // If we were to read a double, then stop reading from input, the function would accept
    // this input even though it is invalid. (and the input buffer won't be empty)
    // We must also consider decimal values, very large values and very long strings so as
    // to avoid overflow issues.
    int num_read_success = scanf("%lf", &num_input);

    for (int i = 0; i < (INPUT_SIZE - 1); i++) {
        if ((temp = getchar()) != ' ' && temp != '\n' && temp != EOF)
            char_input[num_chars++] = temp;
        else if (temp == '\n' || temp == EOF)
            break;
        if (temp != '\n' && i == (INPUT_SIZE - 2)) 
            while (getchar() != '\n'); // Clear input buffer if input contains too many chars

    } char_input[num_chars] = '\0'; // Must add terminating character

    switch (ValidNumber(num_read_success, num_chars, num_input)) {
        case 1: // true
            return num_input;
        case 0: // false
            return GetInput();
        case -1: // input does not contain a number
            break;
    }

    if (strcmp(char_input, "exit") == 0) {
        return EXIT;
    } else if (strcmp(char_input, "restart") == 0) {
        return AGAIN;
    } else {
        printf("Invalid string input.\n");
        return GetInput();
    } 
}

void PlayPiece(int board[H][W], position p, int piece) {
    board[p / H][p - W * (p / W)] = piece;
    // utilizes integer division to get the correct indices
    // ex: 7/3 is 2, 7 - 3*(7/3) is 1
}

gameState Player(int board[H][W]) {
    int spots_length;
    int *_valid_spots = _GetValidSpots(board, &spots_length);

    // Input loop
    while (1) {
        DisplayBoard(board);
        printf("Enter a number that exists on the board, or <restart>, or <exit>:\n");

        int input = GetInput();
        if (input == EXIT || input == AGAIN)
            return input;
        
        if (IsIn(input, _valid_spots, spots_length) == true) {
            PlayPiece(board, input, PLAYER);
            break;
        } else {
            printf("Spot '%d' is taken.\n", input);
        }
    }
    
    gameEvent result = CheckGameOver(board, spots_length-1); // Subtract 1 since we have just made a move
    if (result == CONTINUE) {
        return Bot(board);
    } else {
        DisplayBoard(board);
        return GameOver(result);
    }
}

int main() {
    gameState state = AGAIN;
    if (H != W | H > 9 | W > 9) {
        printf("Invalid dimensions. Must be equal and less than or equal to 9.\n");
        state = EXIT;
    }

    while (state != EXIT) {
        int board[H][W]; // Initialize board
        int num = 0;
        for (int i = 0; i < W; i++) {
            for (int j = 0; j < H; j++) {
                board[i][j] = num++;
            }
        }
        state = Player(board);
    }
    return EXIT_SUCCESS;
}