#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAX_NOTE 14

void clearTerminal();
void enableRawMode();
void disableRawMode();
char getInput();
void SIGINThandler();
void handleCleanup();
void moveNotes(char input);
void createNote();
void printgame();

volatile sig_atomic_t sigint_received = 0; // Flag for SIGINT

typedef struct Note{
    char dir;
    int place;
} Note;

char directions[4] ={'a', 's', 'd', 'f'};
Note* notes[MAX_NOTE];
int size = 0;
int start = 0;
int score = 0;
int fails = 0;

char* loseScreen = "********************************************************************\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                             YOU LOSE!                            *\n"
"*                             RETURNING                            *\n"
"*                              TO MAIN                             *\n"
"*                              SCREEN                              *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*            ^            ->            <-            |            *\n"
"*            |                                        v            *\n"
"*            A            S             D             F            *\n"
"********************************************************************\n";

char* winScreen = "********************************************************************\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                             YOU WIN!                             *\n"
"*                             RETURNING                            *\n"
"*                              TO MAIN                             *\n"
"*                              SCREEN                              *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*                                                                  *\n"
"*            ^            ->            <-            |            *\n"
"*            |                                        v            *\n"
"*            A            S             D             F            *\n"
"********************************************************************\n";
char* dir_q = "*            ^                                                     *\n"
"*            |                                                     *\n";

char* dir_w = "*                         ->                                       *\n"
"*                                                                  *\n";
char* dir_e = "*                                       <-                         *\n"
"*                                                                  *\n";
char* dir_r = "*                                                     |            *\n"
"*                                                     v            *\n";

int main(){
        system("clear");
        clearTerminal();
        signal(SIGINT, SIGINThandler);
        signal(SIGTERM, SIGINThandler);

        srand(time(0));

        enableRawMode();

        int x = 0;

        createNote();
        while(1){

            char input = getInput();

            if (input == 'q' || sigint_received == 1) {
                break;
            }
            moveNotes((int)input);
            if(score <= -1000){
                system("clear");
                clearTerminal();
                printf("%s", loseScreen);
                sleep(3);
                break;
            }
            if(score >= 500){
                
                printf("%s", winScreen);
                sleep(3);
                break;
            }
            system("clear");
            clearTerminal();
            printgame();

            input = '\0';
            usleep(600000);
        }

    handleCleanup();
    return 0;
}



// Function to get input from the user
char getInput() {
    char input = '\0'; // Default value for no input
    read(STDIN_FILENO, &input, 1);  // Read 1 character if available
    return input;
}

// Function to //clear the terminal
void clearTerminal() {
    // Moves cursor to the home position and //clears the screen
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

void SIGINThandler() {
    sigint_received = 1;
}

void moveNotes(char input){
    for (int i = start ; i < start + size; i ++){
        Note *note = notes[i % MAX_NOTE];
        (*note).place--;

        if(size == 14){
            if((*note).dir == input){
                score += 20;
            }
            else{
                fails++;
                score -= 10;
            }
            notes[i] == NULL;
            start = (start + 1) % MAX_NOTE;
            size --;
        }
    }
    if(size < MAX_NOTE){
            createNote();
        }
}

void createNote(){
    Note *newNote = (Note*) malloc(sizeof(Note));
    int newDir = rand() % 4;
    newNote->dir = directions[newDir];
    newNote->place = MAX_NOTE * 2;
    notes[(start + size) % MAX_NOTE] = newNote;
    size ++;
}


void handleCleanup() {
    for(int i = start; i < (start + size) % MAX_NOTE; size ++){
        free(notes[i % MAX_NOTE]);
        notes[i % MAX_NOTE] = NULL;
    }
  disableRawMode();

}

void printgame(){
    for(int i = 0; i < 68; i++){
        printf("*");
    }
    printf("\n*                            GUITAR HERO           score=");
    if(score >= 0){
        if(score < 100){
            printf("0");
        }
        if(score < 10){
            printf("0");
        }
    }
    printf("%d%s", score, "       *\n*                                                                  *\n");
    for(int i = (start + size) - 1; i > start ; i--){
        Note *note = notes[i % MAX_NOTE];
        if(note->place == 0){
            continue;
        }
        if((*note).dir == 'a'){
            printf("%s", dir_q);
        }
        else if((*note).dir == 's'){
            printf("%s", dir_w);
        }
        else if((*note).dir == 'd'){
            printf("%s", dir_e);
        }
        else{
            printf("%s", dir_r);
        }
    }
    for(int i = 0; i < MAX_NOTE - size; i++){
        printf("%s", "*                                                                  *\n*                                                                  *\n");
    }
    printf("%s", "*            ^            ->            <-            |            *\n*            |                                        v            *\n");
    printf("%s", "*            A            S             D             F            *\n********************************************************************\n");
    fflush(stdout);
}