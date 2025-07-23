#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "io_helpers.h"
#include "builtins.h"


// ===== Output helpers =====

/* Prereq: str is a NULL terminated string
 */
void display_message(char *str) {
    write(STDOUT_FILENO, str, strnlen(str, MAX_STR_LEN));
}


/* Prereq: pre_str, str are NULL terminated string
 */
void display_error(char *pre_str, char *str) {
    write(STDERR_FILENO, pre_str, strnlen(pre_str, MAX_STR_LEN));
    write(STDERR_FILENO, str, strnlen(str, MAX_STR_LEN));
    write(STDERR_FILENO, "\n", 1);
}


// ===== Input tokenizing =====

/* Prereq: in_ptr points to a character buffer of size > MAX_STR_LEN
 * Return: number of bytes read
 */
ssize_t get_input(char *in_ptr) {
    int retval = read(STDIN_FILENO, in_ptr, MAX_STR_LEN+1); // Not a sanitizer issue since in_ptr is allocated as MAX_STR_LEN+1
    int read_len = retval;
    if (retval == -1) {
        read_len = 0;
    }
    if (read_len > MAX_STR_LEN) {
        read_len = 0;
        retval = -1;
        write(STDERR_FILENO, "ERROR: input line too long\n", strlen("ERROR: input line too long\n"));
        int junk = 0;
        while((junk = getchar()) != EOF && junk != '\n');
    }
    in_ptr[read_len] = '\0';
    return retval;
}

/* Prereq: in_ptr is a string, tokens is of size >= len(in_ptr)
 * Warning: in_ptr is modified
 * Return: number of tokens.
 */
size_t tokenize_input(char *in_ptr, char **tokens) {
    char *curr_ptr = strtok (in_ptr, DELIMITERS);
    size_t token_count = 0;

    while (curr_ptr != NULL) {
        // replace all variables in token
        char *new_token = malloc(MAX_STR_LEN);
        if (new_token == NULL) {
            while (token_count > 0) {
                free(tokens[--token_count]);  // Clean up prior allocations on failure
            }
            return 0;
        }
        
        int new_len = replace_variables(curr_ptr, new_token, 0, MAX_STR_LEN);
        new_token[new_len] = '\0';

	    tokens[token_count++] = curr_ptr;
        curr_ptr = strtok(NULL, DELIMITERS);
    }
    tokens[token_count] = NULL;
    return token_count;
}


char* replace_variables(char *token, char *final, int curr_final_index, int max_str_len) {
    char *buffer;  // For strtok_r state
    char *var_tokens = strtok_r(token, "$", &buffer);

    // Skip the first segment if token doesn't start with $
    if (token[0] != '$') {
        if (var_tokens != NULL) {
            int len = strlen(var_tokens);
            if (curr_final_index + len >= max_str_len - 1) {
                len = max_str_len - 1 - curr_final_index;
            }
            strncpy(final + curr_final_index, var_tokens, len);
            curr_final_index += len;
        }
        var_tokens = strtok_r(NULL, "$", &buffer);
    }

    // Process each $-delimited variable
    while (var_tokens != NULL) {
        char *val = get_var(var_tokens);
        if (val != NULL) { // Ensure variable exists
            int len = strlen(val);
            if (curr_final_index + len >= max_str_len - 1) {
                len = max_str_len - 1 - curr_final_index;  // Truncate if necessary
            }
            strncpy(final + curr_final_index, val, len);
            curr_final_index += len;
        }
        var_tokens = strtok_r(NULL, "$", &buffer);
    }

    return curr_final_index;
}