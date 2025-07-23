#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>

#include "builtins.h"
#include "io_helpers.h"

static void ctrlCHandler(int signo){
    (void) signo;
    write(STDOUT_FILENO, "\n", 1);
    display_message("mysh$ ");
}

// You can remove __attribute__((unused)) once argc and argv are used.
int main(__attribute__((unused)) int argc, 
         __attribute__((unused)) char* argv[]) {
    char *prompt = "mysh$ ";
    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};

    // variable list
    
signal(SIGINT, ctrlCHandler);
    while (1) {
        // Prompt and input tokenization

        // Display the prompt via the display_message function.
        display_message(prompt);


        int ret = get_input(input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr);

        // Clean exit
        // TODO: The next line has a subtle issue.	// if(token_arr[0] != NULL && strncmp("exit", token_arr[0], 4) == 0 && token_arr[0][4] == '\0'){printf("true");}
        if(ret == 0){
            display_message("\n");
            return 0;
        }
        //line too long err
        if (ret == -1){
            continue;
        }

        if (token_count == 0){
            continue;
        }
        if ((strncmp("exit", token_arr[0], 4) == 0) && token_arr[0][4] == '\0') {
            
            return 0;
	    }

        // Command execution
        if (strchr(token_arr[0], '=') != NULL){

            char *name = strtok(token_arr[0], "=");
            char *val = strtok(NULL, "");
            if (val == NULL){
                continue;
            }
            def_var(name, val);
            
            continue;
            
        }

        if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);
            if (builtin_fn != NULL) {
                ssize_t err = builtin_fn(token_arr);
                if (err == -1) {
                    display_error("ERROR: Builtin failed: ", token_arr[0]);
                }
            } else {
                external(token_arr, token_count);
            }
        }

        // process parts of pipes by separating by '|'
        // char *commands[32];  // Array to store separated commands
        // int command_count = 0;  // Keeps track of the number of commands
        // char *current_command = NULL;
        // size_t i = 0;

        // // Process tokens before and after the pipe
        // while (i < token_count) {
        //     if (strcmp(token_arr[i], "|") == 0) {
        //         // Store the current command (before pipe) if it exists
        //         if (current_command != NULL) {
        //             commands[command_count++] = current_command;
        //             current_command = NULL;
        //         }
        //         // Skip over the pipe token
        //         i++;
        //         continue;
        //     }

        //     // Accumulate tokens to form a command
        //     if (current_command == NULL) {
        //         current_command = strdup(token_arr[i]);
        //     } else {
        //         current_command = realloc(current_command, strlen(current_command) + strlen(token_arr[i]) + 2);
        //         strcat(current_command, " ");
        //         strcat(current_command, token_arr[i]);
        //     }

        //     i++;
        //     }

        //     // If there's a remaining command after the loop, add it
        //     if (current_command != NULL) {
        //         commands[command_count++] = current_command;
        //     }

        //     // Print the separated commands
        //     for (int i = 0; i < command_count; i++) {
        //         printf("Command %d: '%s'\n", i + 1, commands[i]);
        //     }

        //     if (command_count >=2){
        //         ;
        //     }





        // Free the tokens
        for (size_t i = 0; i < token_count; i++) {
            free(token_arr[i]);
        }

        

        
    }
        

    return 0;
}
