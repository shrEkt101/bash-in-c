#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include <unistd.h>



/* Type for builtin handling functions
 * Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
typedef ssize_t (*bn_ptr)(char **);


ssize_t bn_ls(char **tokens);
ssize_t list_directory(char *path, int depth, char *filter);
ssize_t bn_echo(char **tokens);
ssize_t word_count(char **tokens);
ssize_t cat(char **tokens);
ssize_t cd(char **tokens);
ssize_t pwd();

typedef struct Variable{
    char *name;
    char *val;
    struct Variable *next;
} Variable;

extern Variable *vars;

int def_var(char *name, char *val);
char *get_var(char *name);
void free_var();


/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd);


/* BUILTINS and BUILTINS_FN are parallel arrays of length BUILTINS_COUNT
 */
static const char * const BUILTINS[] = {"echo", "wc", "cat", "cd", "pwd", "ls"};
static const bn_ptr BUILTINS_FN[] = {bn_echo, word_count, cat, cd, pwd, bn_ls, NULL};    // Extra null element for 'non-builtin'
static const ssize_t BUILTINS_COUNT = sizeof(BUILTINS) / sizeof(char *);

#endif
