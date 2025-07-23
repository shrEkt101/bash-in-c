#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
#include "io_helpers.h"


Variable *vars = NULL;

// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;

    char final[MAX_STR_LEN];
    int curr_final_index = 0;

    while (tokens[index] != NULL) {

        // add space to final
        if (index > 1){
            char space = ' ';
            int len = 1;

            if (curr_final_index + len >= MAX_STR_LEN - 1) {
                len = MAX_STR_LEN - 1 - curr_final_index;
            }
            strncpy(final + curr_final_index, &space,len);
            curr_final_index++;
        }
        
        // specific token
        char *token = tokens[index];

        //if lone $ found simply print $ and move on
        if (strlen(token) == 1 && token[0] == '$'){
            char dol = '$';
            int len = 1;

            if (curr_final_index + len >= MAX_STR_LEN - 1) {
                len = MAX_STR_LEN - 1 - curr_final_index;
            }
            strncpy(final + curr_final_index, &dol,len);
            curr_final_index++;
        }
        
        // add current token to expanded
        // check if character starts with $ followed by alphanum or _
        // 

        // save one space for null terminator

        // keep checking each character for $ or null terminator:

        int skip = 0;
        if (token[0] != '$'){
            skip = 1;
        }

        char * buffer;
        // contains everything before the first $
        char *var_tokens = strtok_r(token, "$", &buffer);


        // if there is a string before the first $
        if (skip){
            if (var_tokens != NULL){
                int len = strlen(var_tokens);
                strncpy(final + curr_final_index, var_tokens, len);
                curr_final_index += len;
            }
            var_tokens = strtok_r(NULL, "$", &buffer);
            // printf("%s skipped\n", var_tokens);
        }

        if (var_tokens != NULL){
            while(var_tokens != NULL){
            
                // save one space for null terminator
                // the token ends with '\0', hence treat everything after first char as the string;
                char *val = get_var(var_tokens);
                
                if (val != NULL) { // Ensure variable exists
                    
                    int len = strlen(val);
                    if (curr_final_index + len >= MAX_STR_LEN - 1) {
                        len = MAX_STR_LEN - 1 - curr_final_index;  // Truncate if necessary
                    }
                    strncpy(final + curr_final_index, val, len);
                    curr_final_index += len;
                }
                var_tokens = strtok_r(NULL, "$", &buffer);
            }
            index++;
            continue;
        }
        
        index++;
    }

    final[curr_final_index] = '\0';  // Ensure null termination
    display_message(final);
    display_message("\n");
    return 0;
}


int def_var(char *name, char *val) {
    Variable *curr = vars;

    // printf("about to expand %s\n", val);

    // Buffer to store the expanded value
    char new_val[1024] = {0}; // Adjust size as needed
    int new_val_index = 0;

    // Expand val if $ found
    for (size_t i = 0; i < strlen(val); i++) {
        if (val[i] == '$') {
            // Start of a variable reference
            i++; // Skip the '$'
            char nameBuffer[128] = {0};
            int nameIndex = 0;

            // Extract the variable name
            while (val[i] != '\0' && val[i] != '$' && val[i] != ' ' && val[i] != '\t' && val[i] != '\n') {
                nameBuffer[nameIndex++] = val[i++];
            }
            nameBuffer[nameIndex] = '\0';

            // Look up the variable's value
            char *var_val = get_var(nameBuffer);
            if (var_val) {
                // Append the variable's value to new_val
                strcpy(new_val + new_val_index, var_val);
                new_val_index += strlen(var_val);
            } else {
                // If the variable is not found, append the original reference
                strcpy(new_val + new_val_index, "$");
                new_val_index++;
                strcpy(new_val + new_val_index, nameBuffer);
                new_val_index += strlen(nameBuffer);
            }

            i--; // Adjust for the loop increment
        } else {
            // Append the current character to new_val
            new_val[new_val_index++] = val[i];
        }
    }
    new_val[new_val_index] = '\0'; // Null-terminate the expanded value

    // printf("expanded to %s\n", new_val);

    // Overwrite duplicate
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            free(curr->val);
            curr->val = strdup(new_val); // Use the expanded value
            return 0;
        }
        curr = curr->next;
    }

    // Write new variable
    Variable *new_var = malloc(sizeof(Variable));
    new_var->name = strdup(name);
    new_var->val = strdup(new_val); // Use the expanded value
    new_var->next = vars;
    vars = new_var;

    return 0;
}

char *get_var(char *name){
    
    Variable *curr = vars;
    while(curr != NULL){
        if (strcmp(curr->name, name) == 0){
            // printf("%s found\n", name);
            return curr->val;
        }
        curr=curr->next;
    }
    return NULL;
}

void free_var(){
    Variable *curr = vars;
    Variable *temp = curr;
    while(curr){
        curr = curr->next;

        free(curr->name);
        free(curr->val);
        free(temp);
    }

}


