#include "shell.h"
#include "kernel.h"
#include "keyboard.h"

#define COMMAND_BUFFER_SIZE 256
#define MAX_HISTORY 10

static char command_history[MAX_HISTORY][COMMAND_BUFFER_SIZE];
static int history_count = 0;
static int history_index = 0;

void shell_process_command(const char* command_line);

// Custom strcpy
char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while ((*dest++ = *src++));
    return original_dest;
}

void add_to_history(const char* command_line) {
    if (history_count > 0 && strcmp(command_history[history_count - 1], command_line) == 0) {
        // Don't add duplicate commands
        return;
    }

    if (history_count < MAX_HISTORY) {
        strcpy(command_history[history_count], command_line);
        history_count++;
    } else {
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(command_history[i], command_history[i+1]);
        }
        strcpy(command_history[MAX_HISTORY - 1], command_line);
    }
    history_index = history_count;
}

void shell_run() {
    char command_buffer[COMMAND_BUFFER_SIZE];
    int buffer_index = 0;

    while (1) {
        char c = get_char();

        if (c == '\n') {
            command_buffer[buffer_index] = '\0';
            terminal_print("\n", COLOR_GREEN_ON_BLACK);
            if (buffer_index > 0) {
                add_to_history(command_buffer);
                shell_process_command(command_buffer);
            }
            buffer_index = 0;
            terminal_print("> ", COLOR_GREEN_ON_BLACK);
        } else if (c == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                if (terminal_col > 2) {
                    terminal_col--;
                    terminal_putchar(terminal_col, terminal_row, ' ', COLOR_GREEN_ON_BLACK);
                }
            }
        } else if (c == UP_ARROW) {
            if (history_index > 0) {
                history_index--;
                strcpy(command_buffer, command_history[history_index]);
                buffer_index = strlen(command_buffer);
                
                // Clear the line
                terminal_col = 2; // After "> "
                for(int i = 0; i < buffer_index + 10; i++) { // a bit extra to be safe
                    terminal_putchar(terminal_col + i, terminal_row, ' ', COLOR_GREEN_ON_BLACK);
                }

                terminal_col = 2;
                terminal_print(command_buffer, COLOR_GREEN_ON_BLACK);
            }
        } else if (c == DOWN_ARROW) {
            if (history_index < history_count) {
                history_index++;
                if (history_index == history_count) {
                    // Clear the line for a new command
                    command_buffer[0] = '\0';
                    buffer_index = 0;
                    terminal_col = 2; // After "> "
                    for(int i = 0; i < 80 - 2; i++) { 
                        terminal_putchar(terminal_col + i, terminal_row, ' ', COLOR_GREEN_ON_BLACK);
                    }
                    terminal_col = 2;
                } else {
                    strcpy(command_buffer, command_history[history_index]);
                    buffer_index = strlen(command_buffer);
                    
                    // Clear the line
                    terminal_col = 2; // After "> "
                    for(int i = 0; i < 80 - 2; i++) {
                        terminal_putchar(terminal_col + i, terminal_row, ' ', COLOR_GREEN_ON_BLACK);
                    }

                    terminal_col = 2;
                    terminal_print(command_buffer, COLOR_GREEN_ON_BLACK);
                }
            }
        }
        else if (buffer_index < COMMAND_BUFFER_SIZE - 1) {
            command_buffer[buffer_index++] = c;
            char str[2] = {c, '\0'};
            terminal_print(str, COLOR_GREEN_ON_BLACK);
        }
    }
}

void shell_process_command(const char* command_line) {
    if (strncmp(command_line, "echo", 4) == 0) {
        const char* arg_start = command_line + 4;
        while (*arg_start == ' ') {
            arg_start++;
        }
        terminal_print(arg_start, COLOR_GREEN_ON_BLACK); 
        terminal_print("\n", COLOR_GREEN_ON_BLACK);
    } else if (strcmp(command_line, "help") == 0) {
        terminal_print("Available commands:\n", COLOR_GREEN_ON_BLACK);
        terminal_print("  echo <message> - Prints a message\n", COLOR_GREEN_ON_BLACK);
        terminal_print("  help           - Displays this help message\n", COLOR_GREEN_ON_BLACK);
        terminal_print("  clear          - Clears the screen\n", COLOR_GREEN_ON_BLACK);
        terminal_print("  rps            - Play Rock-Paper-Scissors\n", COLOR_GREEN_ON_BLACK);
        terminal_print("\n", COLOR_GREEN_ON_BLACK);
    } else if (strcmp(command_line, "clear") == 0) {
        terminal_clear(COLOR_GREEN_ON_BLACK);
    } else if (strcmp(command_line, "rps") == 0) {
        terminal_print("Let's play Rock-Paper-Scissors!\n", COLOR_GREEN_ON_BLACK);
        terminal_print("Enter your choice (r, p, or s): ", COLOR_GREEN_ON_BLACK);

        char player_choice = get_char();
        terminal_print("\n", COLOR_GREEN_ON_BLACK);

        if (player_choice != 'r' && player_choice != 'p' && player_choice != 's') {
            terminal_print("Invalid choice! Please enter 'r', 'p', or 's'.\n", COLOR_GREEN_ON_BLACK);
        } else {
            const char* choices[] = {"Rock", "Paper", "Scissors"};
            
            uint32_t computer_num = rand() % 3; 

            terminal_print("You chose: ", COLOR_GREEN_ON_BLACK);
            if (player_choice == 'r') terminal_print("Rock\n", COLOR_GREEN_ON_BLACK);
            else if (player_choice == 'p') terminal_print("Paper\n", COLOR_GREEN_ON_BLACK);
            else if (player_choice == 's') terminal_print("Scissors\n", COLOR_GREEN_ON_BLACK);

            terminal_print("Computer chose: ", COLOR_GREEN_ON_BLACK);
            terminal_print(choices[computer_num], COLOR_GREEN_ON_BLACK);
            terminal_print("\n", COLOR_GREEN_ON_BLACK);

            if ( (player_choice == 'r' && computer_num == 0) ||
                 (player_choice == 'p' && computer_num == 1) ||
                 (player_choice == 's' && computer_num == 2) ) {
                terminal_print("It's a tie!\n", COLOR_GREEN_ON_BLACK);
            } else if ( (player_choice == 'r' && computer_num == 2) ||
                        (player_choice == 'p' && computer_num == 0) ||
                        (player_choice == 's' && computer_num == 1) ) {
                terminal_print("You win!\n", COLOR_GREEN_ON_BLACK);
            } else {
                terminal_print("Computer wins!\n", COLOR_GREEN_ON_BLACK);
            }
        }
        terminal_print("\n", COLOR_GREEN_ON_BLACK);
    }
    else {
        terminal_print("Unknown command: ", COLOR_GREEN_ON_BLACK);
        terminal_print(command_line, COLOR_GREEN_ON_BLACK);
        terminal_print("\n", COLOR_GREEN_ON_BLACK);
    }
}

void shell_init() {
    time_t current_time;
    get_current_time(&current_time);
    srand(current_time.second + current_time.minute * 60 + current_time.hour * 3600);
}

