#ifndef COMMAND_H
#define COMMAND_H
#include "../Token/token.h"

typedef struct {
	tokenlist *tokens;
} command;

command *new_command(void);
void get_commands(command **commands, tokenlist *tokens, int numPipes);
void free_command(command *command);

char **get_args(int *start, tokenlist *tokens);
void add_arg(char **args, char *arg, int currSize);

#endif