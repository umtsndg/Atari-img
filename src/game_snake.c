#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#define ROW_COUNT 15
#define COL_COUNT 15

// Linked list to represent the snake
typedef struct SnakeSegment {
    int x, y;
    struct SnakeSegment *next;
    char* color;
} SnakeSegment;

// Function declarations
void clearTerminal();
void enableRawMode();
void disableRawMode();
char getInput();
void initializeFeed(int *feedX, int *feedY, SnakeSegment *snake);
void renderGrid(SnakeSegment *snake, int feedX, int feedY);
void updateSnake(SnakeSegment *snake, char direction, int *validMove);
void growSnake(SnakeSegment *snake);
int checkCollision(SnakeSegment *snake, int x, int y);
void SIGINThandler();

SnakeSegment *global_snake = NULL;
volatile sig_atomic_t sigint_received = 0; // Flag for SIGINT
char* colorArray[] = {"\033[91m", "\033[92m", "\033[93m", "\033[94m", "\033[95m", "\033[96m", "\033[97m"};
int ind = 0;

int main() {
    system("clear");
    clearTerminal();
    SnakeSegment *snake = (SnakeSegment *)malloc(sizeof(SnakeSegment));
    snake->color = colorArray[ind];
    ind++;
    global_snake = snake;
    signal(SIGINT, SIGINThandler);
    signal(SIGTERM, SIGINThandler);
    snake->x = COL_COUNT / 2;
    snake->y = ROW_COUNT / 2;
    snake->next = NULL;

    int feedX, feedY;
    srand(time(0)); // Seed the random number generator
    initializeFeed(&feedX, &feedY, snake);

    enableRawMode();
    char direction = 'w';
    int validMove = 1;

    while (1) {
        system("clear");
        clearTerminal();
        renderGrid(snake, feedX, feedY);

        char input = getInput();
        if (input == 'q' || sigint_received == 1) {
            break;
        }

        // Validate input
        if ((input == 'w' && direction != 's') || (input == 'a' && direction != 'd') ||
            (input == 's' && direction != 'w') || (input == 'd' && direction != 'a')) {
            direction = input;
        }

        updateSnake(snake, direction, &validMove);

        if (snake->x == feedX && snake->y == feedY) {
            growSnake(snake);
            fflush(stdout);
            initializeFeed(&feedX, &feedY, snake);
        }
        usleep(120000); // Add a small delay (150ms) to control game speed
}


// Free snake memory
while (snake != NULL) {
    SnakeSegment *temp = snake;
    snake = snake->next;
    free(temp);
}

disableRawMode();
system("clear");
clearTerminal();
fflush(stdout);

return 0; // Return to the caller (main menu)
}

// Function to clear the terminal
void clearTerminal() {
    // Moves cursor to the home position and clears the screen
    printf("\033[H\033[J");
    fflush(stdout);  // Ensures the output is immediately flushed
}



// Function to enable raw mode for immediate input
void enableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    term.c_cc[VMIN] = 0;             // Minimum number of characters for non-blocking
    term.c_cc[VTIME] = 0;            // No waiting time
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


// Function to disable raw mode and restore terminal settings
void disableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to initialize the feed position ensuring it's not overlapping with the snake
void initializeFeed(int *feedX, int *feedY, SnakeSegment *snake) {
    int valid;
    do {
        *feedX = rand() % COL_COUNT;
        *feedY = rand() % ROW_COUNT;
        valid = !checkCollision(snake, *feedX, *feedY);
    } while (!valid);
}

// Function to render the grid
void renderGrid(SnakeSegment *snake, int feedX, int feedY) {
    srand(time(NULL));
    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < COL_COUNT; j++) {
            
            if (i == feedY && j == feedX) {
                // Color the bait in red
                printf("%sX%s ", "\033[91m", "\033[0m");
            } else if (checkCollision(snake, j, i)) {
                if (snake->x == j && snake->y == i) {
                    char* color = colorArray[rand() % 7];
                    printf("%sO%s ", color, "\033[0m");
                } else {
                    char* color = colorArray[rand() % 7];
                    printf("%s#%s ", color, "\033[0m");
                }
            } else {
                // Empty space (no color)
                printf(". ");
            }
        }
        printf("\n");
    }
}


// Function to get input from the user
char getInput() {
    char input = '\0'; // Default value for no input
    read(STDIN_FILENO, &input, 1);  // Read 1 character if available
    return input;
}


// Function to update the snake's position
void updateSnake(SnakeSegment *snake, char direction, int *validMove) {
    int newX = snake->x;
    int newY = snake->y;

    if (direction == 'w') newY--;
    if (direction == 'a') newX--;
    if (direction == 's') newY++;
    if (direction == 'd') newX++;

    // Check for border collisions
    if (newX < 0 || newX >= COL_COUNT || newY < 0 || newY >= ROW_COUNT) {
        *validMove = 0;
        return; // Do not update position
    }

    // Check for self-collision
    if (checkCollision(snake->next, newX, newY)) { // Ignore the head in collision check
        *validMove = 0;
        return; // Do not update position
    }

    // Update position
    SnakeSegment *current = snake;
    int prevX = current->x, prevY = current->y;
    current->x = newX;
    current->y = newY;

    while (current->next != NULL) {
        current = current->next;
        int tempX = current->x, tempY = current->y;
        current->x = prevX;
        current->y = prevY;
        prevX = tempX;
        prevY = tempY;
    }
    *validMove = 1;
}


// Function to grow the snake
void growSnake(SnakeSegment *snake) {
    SnakeSegment *current = snake;
    while (current->next != NULL) {
        current = current->next;
    }

    SnakeSegment *newSegment = (SnakeSegment *)malloc(sizeof(SnakeSegment));
    newSegment->color = colorArray[ind % (sizeof(colorArray) / sizeof(colorArray[0]))];
    ind ++;
    newSegment->x = current->x;
    newSegment->y = current->y;
    newSegment->next = NULL;
    current->next = newSegment;
}

// Function to check if a position collides with the snake
int checkCollision(SnakeSegment *snake, int x, int y) {
    SnakeSegment *current = snake;
    while (current != NULL) {
        if (current->x == x && current->y == y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void SIGINThandler() {
    sigint_received = 1;
}
