#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

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




ssize_t bn_ls(char **tokens) {
    // Check for recursive flag
    int recursive = 0;
    char *error = "-1";
    for (int i = 1; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "--rec") == 0) {
            recursive = 1;
            break;
        }
    }
    
    // Get depth (default to -1 if not recursive, 0 if recursive with no depth)
    int depth = recursive ? 0 : -1;
    for (int i = 1; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "--d") == 0 && tokens[i+1] != NULL) {
            char *endptr;
            depth = strtol(tokens[i+1], &endptr, 10);
            
            // Validate depth
            if (*endptr != '\0' || depth < 0) {
                display_error("Invalid depth value", error);
                return -1;
            }
            break;
        }
    }
    
    // Check that depth is only provided with --rec
    if (!recursive && depth >= 0) {
        display_error("Invalid use of --d: --rec flag not provided", error);
        return -1;
    }
    
    // Get filter string
    char filter[128] = "";
    for (int i = 1; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "--f") == 0 && tokens[i+1] != NULL) {
            strcpy(filter, tokens[i+1]);
            break;
        }
    }
    
    // Get path (default to current directory)
    char path[128] = "./";
    int path_found = 0;
    
    for (int i = 1; tokens[i] != NULL; i++) {
        // Skip known flags and their values
        if (strcmp(tokens[i], "--d") == 0 || strcmp(tokens[i], "--f") == 0) {
            i++; // Skip the flag's value
            continue;
        }
        if (strcmp(tokens[i], "--rec") == 0) {
            continue;
        }
        
        // Found a path argument
        if (path_found) {
            display_error("Too many arguments: ls takes a single path", error);
            return -1;
        }
        strcpy(path, tokens[i]);
        path_found = 1;
    }
    
    // Perform the listing
    return list_directory(path, depth, filter);
}

// Simplified recursive directory listing
ssize_t list_directory(char *path, int depth, char *filter) {
    // Stop if we've reached max depth
    if (depth == 0) {
        return 0;
    }
    
    // Open directory
    DIR *dir = opendir(path);
    if (dir == NULL) {
        char *one="one";
        display_error("Invalid path", one);
        return -1;
    }
    
    // Read directory contents
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        
        // Check if name matches filter
        if (filter[0] == '\0' || strstr(entry->d_name, filter) != NULL) {
            display_message(entry->d_name);
            display_message("\n");
        }

        // Skip . and .. entries if recursive
        if (depth != -1 && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
            continue;
        }
        
        // If recursive, process subdirectories
        if (depth != -1) {
            // Build full path to entry
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            
            // Check if it's a directory
            struct stat st;
            if (lstat(full_path, &st) == 0 && S_ISDIR(st.st_mode) && 
                strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                // Recursively list subdirectory with decremented depth
                list_directory(full_path, depth - 1, filter);
            }
        }
    }
    
    closedir(dir);
    return 0;
}


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


ssize_t word_count(char **tokens){

    int wordCount = 0;
    int charCount = 0;
    int newlineCount = 0;

    if(tokens[1]==NULL){
        display_error("ERROR: No input source provided", tokens[0]);
        return -1;
    }

    FILE *file = fopen(tokens[1], "r");
    if (file == NULL){
        display_error("ERROR: Cannot open file", tokens[0]);
        return -1;
    }

    char curr;
    char prev = EOF;
    while((curr = fgetc(file))!= EOF){

        if(!((prev=='\n') || prev=='\t' || prev=='\r' || prev==' ')){
            if((curr=='\n') || curr=='\t' || curr=='\r' || curr==' '){
                wordCount++;
            }
        }if(curr == '\n'){
            newlineCount++;
        }
        charCount++;
        prev = curr;
    }

    // create final string to print out
    char final[128];
    // sprintf(final, "word count %d\ncharacter count%d\nnewline count%d", wordCount, charCount, newlineCount);

    // return final;
    sprintf(final, "word count %d\ncharacter count %d\nnewline count %d", wordCount, charCount, newlineCount);
    display_message(final);
    display_message("\n");
    return 0;
    
}

ssize_t cat(char **tokens){

    if(tokens[1]==NULL){
        display_error("ERROR: No input source provided", tokens[0]);
        return -1;
    }

    FILE *file = fopen(tokens[1], "r");
    if (file == NULL){
        display_error("ERROR: Cannot open file", tokens[0]);
        return -1;
    }
    char temp[128];
    while(fgets(temp, MAX_STR_LEN, file)!= NULL){
        display_message(temp);
    }
    display_message("\n");

    return 0;
}

ssize_t cd(char **tokens){

    if(tokens[1]==NULL){
        display_error("ERROR: Invalid path", tokens[0]);
        return 1;
    }
    

    if(chdir(tokens[1])){
        display_error("ERROR: Invalid path", tokens[0]);
        return 1;
    }

    return 0;
}

ssize_t pwd(){
    char path[128];
    if(getcwd(path, sizeof(path))!=NULL){
        display_message(path);
        display_message("\n");
        return 0;
    }
    display_error("ERROR: Faileed to get current working directory", path);
    return 1;
}