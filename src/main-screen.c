#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

// Constants
#define TOTAL_WIDTH 108
#define INNER_WIDTH (TOTAL_WIDTH - 2)
#define MAX_FILENAME_LENGTH 32
#define PREFIX "game_" // Prefix to filter filenames

// ASCII Art Constants (Unchanged)
const char* arrow1 =
    "**************************************************************************"
    "************************************\n"
    "*                                                                         "
    "                                   *\n"
    "*  H   H      OOO      PPPP      EEEEE      SSS      TTTTT      A      "
    "TTTTT     III      OOO      N   N     *\n"
    "*  H   H     O   O     P   P     E         S           T       A A       "
    "T        I      O   O     NN  N     *\n"
    "*  HHHHH     O   O     PPPP      EEEE       SSS        T      AAAAA      "
    "T        I      O   O     N N N     *\n"
    "*  H   H     O   O     P         E             S       T      A   A      "
    "T        I      O   O     N  NN     *\n"
    "*  H   H      OOO      P         EEEEE      SSS        T      A   A      "
    "T       III      OOO      N   N     *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                    33333   111   000   000              "
    "                                   *\n"
    "*                                       33    11  0   0 0   0             "
    "                                   *\n"
    "*                                    33333    11  0   0 0   0             "
    "                                   *\n"
    "*                                       33    11  0   0 0   0             "
    "                                   *\n"
    "*                                    33333   111   000   000              "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                     WELCOME TO HOPESTATION              "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                    start                                                "
    "          exit                     *\n"
    "*                      ^                                                  "
    "                                   *\n"
    "*                     / \\                                                "
    "                                    *\n"
    "*                     | |                                                 "
    "                                   *\n"
    "*                     |_|                                                 "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "**************************************************************************"
    "************************************\n\n"
    "                          A To Go Left               D To Go Right        Enter To Select\n";

const char* arrow2 =
    "**************************************************************************"
    "************************************\n"
    "*                                                                         "
    "                                   *\n"
    "*  H   H      OOO      PPPP      EEEEE      SSS      TTTTT      A      "
    "TTTTT     III      OOO      N   N     *\n"
    "*  H   H     O   O     P   P     E         S           T       A A       "
    "T        I      O   O     NN  N     *\n"
    "*  HHHHH     O   O     PPPP      EEEE       SSS        T      AAAAA      "
    "T        I      O   O     N N N     *\n"
    "*  H   H     O   O     P         E             S       T      A   A      "
    "T        I      O   O     N  NN     *\n"
    "*  H   H      OOO      P         EEEEE      SSS        T      A   A      "
    "T       III      OOO      N   N     *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                    33333   111   000   000              "
    "                                   *\n"
    "*                                       33    11  0   0 0   0             "
    "                                   *\n"
    "*                                    33333    11  0   0 0   0             "
    "                                   *\n"
    "*                                       33    11  0   0 0   0             "
    "                                   *\n"
    "*                                    33333   111   000   000              "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                     WELCOME TO HOPESTATION              "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                    start                                                "
    "          exit                     *\n"
    "*                                                                         "
    "           ^                       *\n"
    "*                                                                         "
    "          / \\                      *\n"
    "*                                                                         "
    "          | |                      *\n"
    "*                                                                         "
    "          |_|                      *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "**************************************************************************"
    "************************************\n\n"
    "                          A To Go Left               D To Go Right        Enter To Select\n";

const char* main_screen_first =
    "**************************************************************************"
    "************************************\n"
    "*                                                                         "
    "                                   *\n"
    "*  H   H      OOO      PPPP      EEEEE      SSS      TTTTT      A      "
    "TTTTT     III      OOO      N   N     *\n"
    "*  H   H     O   O     P   P     E         S           T       A A       "
    "T        I      O   O     NN  N     *\n"
    "*  HHHHH     O   O     PPPP      EEEE       SSS        T      AAAAA      "
    "T        I      O   O     N N N     *\n"
    "*  H   H     O   O     P         E             S       T      A   A      "
    "T        I      O   O     N  NN     *\n"
    "*  H   H      OOO      P         EEEEE      SSS        T      A   A      "
    "T       III      OOO      N   N     *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                    33333   111   000   000              "
    "                                   *\n"
    "*                                       33    11  0   0 0   0             "
    "                                   *\n"
    "*                                    33333    11  0   0 0   0             "
    "                                   *\n"
    "*                                       33    11  0   0 0   0             "
    "                                   *\n"
    "*                                    33333   111   000   000              "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                     WELCOME TO HOPESTATION              "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n"
    "*                                                                         "
    "                                   *\n";

// Global Variables (Only for signal handling)
volatile sig_atomic_t sigint_received = 0;  // Flag for SIGINT
char* game;
int gameRunning = 0;

// Function Prototypes (Unchanged)
void clearTerminal();
void enableRawMode();
void disableRawMode();
char getInput();
void SIGINThandler(int signum);
char** getFileList(int* file_count);
void freeFileList(char** fileList, int file_count);
void displayInitialScreen(int choose);
void displayMainMenu(char** fileList, int file_count, int selected_index);
int calculateTotalLength(char* fileList[], int file_count,
                         const char* exit_option);
void handleCleanup(char** fileList, int file_count);


/**
 * Main function orchestrating the program flow.
 */
int main() {
  // Setup signal handler using signal()
  signal(SIGINT, SIGINThandler);
  signal(SIGTERM, SIGINThandler);

  enableRawMode();

  // Retrieve the list of executable files
  int file_count = 0;
  char** fileList = getFileList(&file_count);

  // Add 'exit' option
  const char* exit_option = "exit";
  int total_items = file_count + 1;  // filenames + exit

  // Calculate total available spaces
  int total_name_size = calculateTotalLength(fileList, file_count, exit_option);
  int total_spaces = INNER_WIDTH - total_name_size;
  if (total_spaces < (total_items - 1)) {
    total_spaces = 0;  // Prevent negative spacing
  }

  int gaps = total_items - 1;
  int space_per_gap = gaps > 0 ? total_spaces / gaps : 0;
  int extra_spaces = gaps > 0 ? total_spaces % gaps : 0;

  // Main Loop Variables
  int state = 0;     // 0: Initial Screen, 1: Main Menu
  int choose_1 = 0;  // Selection index for initial screen (0: start, 1: exit)
  int selected_index = 0;  // Selected option index in Main Menu


  while (1) {
    if (sigint_received) {
      // Perform cleanup and exit silently
      handleCleanup(fileList, file_count);
      exit(EXIT_SUCCESS);
    }
    system("clear");
    clearTerminal();
    fflush(stdout);
    if (state == 0) {
      // Display the initial screen with animation
      system("clear");
      clearTerminal();
      displayInitialScreen(choose_1);
      usleep(12000);

      // Handle user input
      char input = getInput();
      if (input == 'q' || input == 'Q') {
        break;
      } else if (input == 'a' || input == 'A') {
        choose_1 = 0;
      } else if (input == 'd' || input == 'D') {
        choose_1 = 1;
      } else if (input == '\n') {
        if (choose_1 == 1) {
          // Exit selected
          break;
        } else {
          // Start selected
          state = 1;
          selected_index = 0;  // Reset selection when entering main menu
        }
      }
    } else if(state == 1) {
      // Display the main menu
      system("clear");
      clearTerminal();
      displayMainMenu(fileList, file_count, selected_index);
      usleep(12000);  // 12 ms

      // Handle user input for main menu
      char input = getInput();
      if (input == 'q' || input == 'Q') {
        break;
      } else if (input == 'a' || input == 'A') {
        // Move selection to the left
        if (selected_index > 0) {
          selected_index--;
        }
      } else if (input == 'd' || input == 'D') {
        // Move selection to the right
        if (selected_index < file_count) {  // Include 'exit' option
          selected_index++;
        }
      } else if (input == '\n') {
        if (selected_index == file_count) {
          // Exit option selected
          state = 0;  // Return to the initial screen
          continue;
        } else {
          char game_path[256] = "";
          // Game selected, execute it
          strcat(game_path, "./");
          strcat(game_path, fileList[selected_index]);
          game = fileList[selected_index];
          gameRunning = 1;
          disableRawMode();
          system(game_path);
          enableRawMode();
          gameRunning = 0;
        }
      }
    }
    usleep(100000);
  }

  // Cleanup before exiting
  handleCleanup(fileList, file_count);
  return 0;
}

// Function Implementations

/**
 * Clears the terminal screen.
 */
void clearTerminal() {
  printf("\033[H\033[J");
  fflush(stdout);
}

/**
 * Enables raw mode for terminal input.
 */
void enableRawMode() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
  term.c_cc[VMIN] = 0;   // Minimum number of characters for non-blocking
  term.c_cc[VTIME] = 0;  // No waiting time
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to disable raw mode and restore terminal settings
void disableRawMode() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

/**
 * Signal handler for SIGINT.
 * Sets the sigint_received flag to indicate interruption.
 */
void SIGINThandler(int signum) {
  if(gameRunning == 1){
    char command[256];
    snprintf(command, sizeof(command), "ps -e | grep %s | grep -v grep | awk '{print$1}' | while read pid; do kill $pid; done", game);
    system(command);
  }
  sigint_received = 1; 
}

/**
 * Reads a single character input from the user.
 * @return The input character.
 */
char getInput() {
  char input = '\0';
  ssize_t bytesRead = read(STDIN_FILENO, &input, 1);

  return input;
}

/**
 * Retrieves the list of executable files from the current directory.
 * @param file_count Pointer to store the number of files found.
 * @return Array of file names.
 */
char** getFileList(int* file_count) {
    char* cwd = NULL;

    // Dynamically allocate memory for the current working directory
    cwd = getcwd(NULL, 0); // Let getcwd allocate the buffer
    if (cwd == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    // Open the current directory
    DIR* dir = opendir(cwd);
    free(cwd); // Free the cwd buffer
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    int count = 0;

    // First pass: Count valid files that start with PREFIX
    while ((entry = readdir(dir)) != NULL) {
        // Check if the filename starts with PREFIX
        if (strncmp(entry->d_name, PREFIX, strlen(PREFIX)) == 0) {
            count++;
        }
    }

    // Allocate memory for the file list
    char** fileList = malloc(count * sizeof(char*));
    if (fileList == NULL) {
        closedir(dir);
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Second pass: Store filenames that start with PREFIX
    rewinddir(dir);
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, PREFIX, strlen(PREFIX)) == 0) {
            fileList[i] = malloc(MAX_FILENAME_LENGTH);
            if (fileList[i] == NULL) {
                // Free previously allocated memory in case of failure
                for (int j = 0; j < i; j++) {
                    free(fileList[j]);
                }
                free(fileList);
                closedir(dir);
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strncpy(fileList[i], entry->d_name, MAX_FILENAME_LENGTH - 1);
            fileList[i][MAX_FILENAME_LENGTH - 1] = '\0'; // Ensure null-termination
            i++;
        }
    }

    closedir(dir);
    *file_count = count;
    return fileList;
}

/**
 * Frees the memory allocated for the file list.
 * @param fileList Array of file names.
 * @param file_count Number of files.
 */
void freeFileList(char** fileList, int file_count) {
  for (int i = 0; i < file_count; i++) {
    free(fileList[i]);
  }
  free(fileList);
}

/**
 * Displays the initial screen with animated arrows.
 * @param choose Current selection (0 for start, 1 for exit).
 */
void displayInitialScreen(int choose) {
  if (choose == 0) {
    printf("%s", arrow1);
    fflush(stdout);
  } else if(choose == 1) {
    printf("%s", arrow2);
    fflush(stdout);
  }
}

/**
 * Displays the main menu with the list of executable files and the 'exit'
 * option. Includes an arrow pointing to the selected option.
 * @param fileList Array of file names.
 * @param file_count Number of files.
 * @param selected_index Currently selected option index.
 */
void displayMainMenu(char** fileList, int file_count, int selected_index) {
  printf("%s", main_screen_first);

  const char* exit_option = "exit";
  int total_items = file_count + 1;  // Files + exit
  int total_name_size = calculateTotalLength(fileList, file_count, exit_option);

  // Calculate available spaces
  int total_spaces = INNER_WIDTH - total_name_size;
  if (total_spaces < (total_items - 1)) {
    total_spaces = 0;  // Prevent negative spacing
  }

  int gaps = total_items - 1;
  int space_per_gap = gaps > 0 ? total_spaces / gaps : 0;
  int extra_spaces = gaps > 0 ? total_spaces % gaps : 0;

  // Build the formatted line and track the start positions of each option
  char formatted_line[INNER_WIDTH + 1];
  formatted_line[0] = '\0';  // Initialize empty string

  // Array to store the starting index of each option
  int* start_positions = malloc(total_items * sizeof(int));
  if (start_positions == NULL) {
    exit(EXIT_FAILURE);
  }

  int current_pos = 0;
  for (int k = 0; k < total_items; k++) {
    // Record the start position
    start_positions[k] = current_pos;

    // Append filename or exit option
    if (k < file_count) {
      strcat(formatted_line, fileList[k]);
      current_pos += strlen(fileList[k]);
    } else {
      strcat(formatted_line, exit_option);
      current_pos += strlen(exit_option);
    }

    // Append spaces after each item except the last one
    if (k < total_items - 1) {
      int spaces_to_add = space_per_gap + (k < extra_spaces ? 1 : 0);
      for (int s = 0; s < spaces_to_add; s++) {
        strcat(formatted_line, " ");
        current_pos++;
      }
    }
  }

  // Ensure the line is exactly INNER_WIDTH characters
  int formatted_length = strlen(formatted_line);
  if (formatted_length < INNER_WIDTH) {
    // Pad the remaining spaces
    while (formatted_length < INNER_WIDTH) {
      strcat(formatted_line, " ");
      formatted_length++;
    }
  } else if (formatted_length > INNER_WIDTH) {
    // Truncate if necessary
    formatted_line[INNER_WIDTH] = '\0';
  }

  // Print the formatted line within the desired frame
  printf("*%s  *\n", formatted_line);  // Frame with '*' on both ends

  // Now, print the arrow below the options
  // Calculate the center position of the selected option
  int selected_start = start_positions[selected_index];
  const char* selected_option =
      (selected_index < file_count) ? fileList[selected_index] : exit_option;
  int option_length = strlen(selected_option);
  int arrow_center = selected_start +
                     (option_length / 2);  // Removed +1 for accurate alignment

  // Ensure arrow_center is within INNER_WIDTH
  if (arrow_center >= INNER_WIDTH) arrow_center = INNER_WIDTH - 1;

  // Build and print each line of the arrow precisely
  // Line 1: Spaces up to arrow_center, then '^'
  printf("*");
  for (int i = 0; i < INNER_WIDTH; i++) {
    if (i == arrow_center) {
      printf("^");
    } else {
      printf(" ");
    }
  }
  printf("  *\n");

  // Line 2: Spaces up to arrow_center -1, then '/', then '\'
  printf("*");
  for (int i = 0; i < INNER_WIDTH; i++) {
    if (i == (arrow_center - 1)) {
      printf("/");
    } else if (i == (arrow_center + 1)) {
      printf("\\");
    } else {
      printf(" ");
    }
  }
  printf("  *\n");

  // Line 3: Spaces up to arrow_center -1, then '|', then '|'
  printf("*");
  for (int i = 0; i < INNER_WIDTH; i++) {
    if (i == (arrow_center - 1) || i == (arrow_center + 1)) {
      printf("|");
    } else {
      printf(" ");
    }
  }
  printf("  *\n");

  // Line 4: Spaces up to arrow_center -1, then '|', '_', '|'
  printf("*");
  for (int i = 0; i < INNER_WIDTH; i++) {
    if (i == (arrow_center - 1)) {
      printf("|");
    } else if (i == arrow_center) {
      printf("_");
    } else if (i == (arrow_center + 1)) {
      printf("|");
    } else {
      printf(" ");
    }
  }
  printf("  *\n");

  printf(
      "*                                                                       "
      "                                     *\n"
      "*                                                                       "
      "                                     *\n"
      "************************************************************************"
      "************************************\n\n"
      "                          A To Go Left               D To Go Right        Enter To Select\n");

  // Free allocated memory for start_positions
  free(start_positions);



  fflush(stdout);
}

/**
 * Calculates the total length of all filenames plus the exit option.
 * @param fileList Array of file names.
 * @param file_count Number of files.
 * @param exit_option The exit option string.
 * @return Total length of all names.
 */
int calculateTotalLength(char* fileList[], int file_count,
                         const char* exit_option) {
  int total = 0;
  for (int j = 0; j < file_count; j++) {
    total += strlen(fileList[j]);
  }
  total += strlen(exit_option);
  return total;
}

/**
 * Handles cleanup operations such as freeing memory and restoring terminal
 * settings.
 * @param fileList Array of file names.
 * @param file_count Number of files.
 */
void handleCleanup(char** fileList, int file_count) {
  freeFileList(fileList, file_count);
  disableRawMode();
  system("clear");
  clearTerminal();

}