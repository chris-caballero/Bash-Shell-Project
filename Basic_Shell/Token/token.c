#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

tokenlist *new_tokenlist(void) {
	tokenlist *tokens = (tokenlist *) malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->items = (char **) malloc(sizeof(char *));
	tokens->items[0] = NULL; /* make NULL terminated */
	return tokens;
}

void add_token(tokenlist *tokens, char *item) {
	int i = tokens->size;

	tokens->items = (char **) realloc(tokens->items, (i + 2) * sizeof(char *));
	tokens->items[i] = (char *) malloc(strlen(item) + 1);
	tokens->items[i + 1] = NULL;
	strcpy(tokens->items[i], item);

	tokens->size += 1;
}

void free_tokens(tokenlist *tokens) {
	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);
	free(tokens->items);
	free(tokens);
}

tokenlist *get_tokens(char *input) {
	char *buf = (char *) malloc(strlen(input) + 1);
	strcpy(buf, input);

	tokenlist *tokens = new_tokenlist();

	char *tok = strtok(buf, " ");
	while (tok != NULL) {
        if(tok[0] == '$') {
			char *env = (char *) malloc(strlen(tok) + 1);
			int i;
			for(i = 0; i < strlen(tok); i++) {
				if(tok[i] != '/') {
					env[i] = tok[i];
				} else {
					break;
				}
			}
			if(i < strlen(tok)) {
				char *var = getenv(++env);
				char *expansion = (char *) malloc(strlen(tok) + strlen(var));
				strcpy(expansion, var);	
				strcat(expansion, tok + i);	
				tok = expansion;		
			}
			tok++;
			tok = getenv(tok) == NULL ? tok : getenv(tok);
        } else if(tok[0] == '~') {
			char *home = getenv("HOME");
			char *expansion = (char *) malloc(strlen(tok) + strlen(home));
			strcpy(expansion, home);
			strcat(expansion, ++tok);
			tok = expansion;
		}
		add_token(tokens, tok);
		tok = strtok(NULL, " ");
	}

	free(buf);
	return tokens;
}