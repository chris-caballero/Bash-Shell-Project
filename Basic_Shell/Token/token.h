#ifndef TOKEN_H
#define TOKEN_H

typedef struct {
	int size;
	char **items;
} tokenlist;

tokenlist *new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);
tokenlist *get_tokens(char *input);

#endif