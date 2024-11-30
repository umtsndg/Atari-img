#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define ROW_COUNT 15
#define COL_COUNT 29

typedef struct Ball{
    int x, y;
    int vx, vy;
} Ball;

void clear_terminal();
void enableRawMode();
void disableRawMode();
char getInput();
void printGame(Ball* ball, int player, int cpu);
void SIGINThandler();
void moveCPU(int dir);
void movePlayer(char dir);
void moveBall(Ball* ball);
void checkGoal();


volatile sig_atomic_t sigint_received = 0; 

int cpuScore = 0;
int playerScore = 0;
int player = 14;
int cpu = 14;
int win = 0;

int main() {
    system("clear");
    clear_terminal();
    srand(time(0));
    signal(SIGINT, SIGINThandler);
    signal(SIGTERM, SIGINThandler);
    Ball *ball = (Ball*) malloc(sizeof(Ball));
    ball->x = 14;
    ball->y = 7;
    ball->vx = 1;
    ball->vy = 1;
    // Enable raw mode once before the loop
    enableRawMode();
    
    while(1){
        system("clear");
        clear_terminal();
        char input = getInput();
        if (input == 'q' || sigint_received == 1) {
            break;
        }
        moveCPU(rand() % 2);
        movePlayer(input);
        moveBall(ball);
        checkGoal(ball);
        if(playerScore == 3){
            win = -1;
        }
        if(cpuScore == 3){
            win = 1;
        }
        printGame(ball, player, cpu);
        fflush(stdout);
        if(win != 0){
            sleep(3);
            break;
        }
        

        usleep(120000);
    }
    // Disable raw mode after the loop
    disableRawMode();
    free(ball);
    return 0;
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
    term.c_lflag |= (ICANON | ECHO); // Restore canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to read non-blocking input
char getInput() {
    char input = '\0'; // Default value for no input
    read(STDIN_FILENO, &input, 1);  // Read 1 character if available
    return input;
}

// Function to clear the terminal
void clear_terminal() {
    printf("\033[H\033[J");
    fflush(stdout);
}

void SIGINThandler() {
    sigint_received = 1;
}

void printGame(Ball* ball, int player, int cpu){
    printf("%s", "              PONG\n");
    printf("%s%d%s%d%s", "       \033[94mPlayer:", playerScore, "\033[0m:\033[91m", cpuScore, " CPU\033[0m\n");
    if(win == 0){
        printf("%s", "      First To 3 Wins!\n");
    }
    else if (win == 1){
        printf("%s", "           You Lose!\n");
    }
    else{
        printf("%s", "           You Win!\n");
    }
    if(win == 0){
        printf("\n");
    }
    else{
        printf("%s", "     Returning to Main Menu\n");
        
    }
    printf("%s", "***********         ***********\n");
    for(int i = 0; i < ROW_COUNT; i++){
        printf("*");
        for(int j = 0; j < COL_COUNT; j++){
            if(abs(cpu - j) <= 1 && i == 0){
                printf("%s_%s","\033[91m","\033[0m");
            }
            else if(abs(player - j) <= 1 && i == ROW_COUNT - 1){
                printf("%s_%s","\033[94m","\033[0m");
            }
            else if(ball->x == j && ball->y == i){
                printf("%sO%s","\033[93m","\033[0m");
            }
            else{
                printf(".");
            }
        }
        printf("%s", "*\n");
    }
    printf("%s", "***********         ***********\n            <-     ->\n            A       D");
}

void moveCPU(int dir){
    if(dir == 0 && cpu < COL_COUNT - 2){
        cpu ++;
    }
    else if(dir == 1 && cpu > 1){
        cpu--;
    }
}

void movePlayer(char dir){
    if(dir == 'a' && player > 1){
        player --;
    }
    else if(dir == 'd' && player < COL_COUNT - 2){
        player ++;
    }
}

void moveBall(Ball* ball){
    if(ball->x + ball->vx >= COL_COUNT || ball->x + ball->vx <= -1){
        ball->vx *= -1;
    }
    if(ball->y + ball->vy >= ROW_COUNT || ball->y+ ball->vy <= -1){
        ball->vy *= -1;
    }
    if(ball->x + ball->vx < COL_COUNT && ball->x + ball->vx >= 0){
        ball->x += ball->vx;
    }
    if(ball->y + ball->vy < ROW_COUNT && ball->y + ball->vy >= 0){
        ball->y += ball->vy;
    }
}

void checkGoal(Ball* ball){
    if(ball->y == 1 && abs(ball->x - cpu) <= 1){
        ball->vy *= -1;
        return;
    }
    if(ball->y == ROW_COUNT - 2 && abs(ball->x - player) <= 1){
        ball->vy *= -1;
        return;
    }
    if(ball->x >= 11 && ball->x <= 19 && ball->y == 0){
        playerScore ++;
        ball->x = 14;
        ball->y = 7;
        ball->vy = -1;
        player = 14;
        cpu = 14;
    }
    if(ball->x >= 11 && ball->x <= 19 && ball->y == ROW_COUNT - 1 ){
        cpuScore ++;
        ball->x = 14;
        ball->y = 7;
        ball->vy = 1;
        player = 14;
        cpu = 14;
    }
}