#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

command *new_command(void) {
	command *comm = (command *) malloc(sizeof(command));
	comm->tokens = new_tokenlist();
	return comm;
}

void free_command(command *command) {
	free_tokens(command->tokens);
	free(command);
}

char **get_args(int *start, tokenlist *tokens) {
	char **args = (char **) malloc(tokens->size*sizeof(char *));
	int i, size = 0;
	for(i = *start; i < tokens->size; i++) {
		char *curr = tokens->items[i];
		if(!strcmp(curr, "<") || !strcmp(curr, ">") || !strcmp(curr, "|") || !strcmp(curr, "&") || curr == NULL) {	
			break;
		} else {
			add_arg(args, curr, size++);
		}
	}
	*start = i;
	return args;
}

void add_arg(char **args, char *arg, int currSize) {
	if(!arg) {
		return;
	}
	args[currSize] = (char *) malloc(strlen(arg)+1);
	args[currSize+1] = NULL;
	strcpy(args[currSize], arg);
}

void show_command(command *cmd) {
	for(int i = 0; i < cmd->tokens->size; i++) {
		printf("%s ", cmd->tokens->items[i]);
	}
	printf("\n");
}

void get_commands(command **commands, tokenlist *tokens, int numPipes) {
	int s = 0;
	int *start = &s;
	int prev = 0;
	
	for(int i = 0; i < numPipes; i++) {
		commands[i]->tokens->items = get_args(start, tokens);
		commands[i]->tokens->size = *start-prev;
		*start = *start + 1;
		prev = *start;
	}

}