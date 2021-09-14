#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>

#include "./Token/token.h"
#include "./Command/command.h"
#include "./Jobs/jobs.h"

//GLOBALS
//list of all active jobs being executed in background
static joblist *list;

//signal handler checks for finished child processes
void sig_handler(int sig);

//read input from user (command + args)
char *get_input(void);

//path (part 5)
//parsing path
tokenlist *get_path_tokens(char *path);
bool isPath(char *path); 
char* get_executable_path(char *cmd, tokenlist *paths);
int check_files(char* dir, char *cmd);

//fork and execute (part 6)
//forked processes
void child_process(tokenlist* tokens, command** commands, tokenlist *paths, int *total);
void parent_process(pid_t pid, char* cmdln, int bg, joblist* list);

//part 7 I/O redirection
int input_redirection(tokenlist *tokens);
int output_redirection(tokenlist *tokens);

//part 8 + extra credit (interpret n-pipes)
int count_pipes(tokenlist* tokens);
int link_commands(int fd_in, int fd_out, command *command, tokenlist *paths);
void chain_pipes(int numPipes, command** commands, tokenlist *paths, int *total);

//helper
char* get_cmdln(char* input, tokenlist* tokens, int echo);

int main()
{
	list = new_joblist();

	int total_commands = 0;
	struct timeval start, end;
	gettimeofday(&start, 0);
	while (1) {
		char cwd[100];
		printf("%s@%s : %s >", getenv("USER"), getenv("MACHINE"), getcwd(cwd, 100));

		signal(SIGCHLD, sig_handler);

		char *input = get_input();

		tokenlist *tokens = get_tokens(input);
		tokenlist *paths = get_path_tokens(getenv("PATH"));

		if(!tokens->size) {
			continue;
		} else if (!strcmp(tokens->items[0], "exit")) {
			wait(NULL);
			gettimeofday(&end, 0);
			long elapsed_time = end.tv_sec - start.tv_sec;
			printf("Shell ran for %ld seconds and executed %d commands\n", elapsed_time, total_commands);
			break;
		} else if (!strcmp(tokens->items[0], "jobs")) {
			report_jobs(list);
		} else if(!strcmp(tokens->items[0], "echo")) {
			printf("%s\n", get_cmdln(input, tokens, 1));
			continue;
		} else if(!strcmp(tokens->items[0], "cd")) {
			int change;
			if(tokens->size > 2) {
				printf("Error: Too many arguments supplied to cd\n");
				continue;
			} else if(tokens->size == 1) {
				change = chdir(getenv("HOME"));
			} else {
				change = chdir(tokens->items[1]);
			}
			if(change < 0) {
				printf("Error: Invalid path\n");
				continue;
			} else {
				continue;
			}
		}

		// //for background processing 
		int bg = !strcmp(tokens->items[tokens->size-1], "&");
		char * cmdln = get_cmdln(input, tokens, 0);

		

		int numPipes = count_pipes(tokens) + 1;

		command **commands = (command **) calloc(numPipes, sizeof(command*));
		for(int i = 0; i < numPipes; i++) { commands[i] = new_command(); }
		get_commands(commands, tokens, numPipes);

		
		if(numPipes > 1) {
			//we have pipes...
			pid_t p = fork();
			if(p == 0) {
				chain_pipes(numPipes, commands, paths, &total_commands);
			} else {
				parent_process(p, cmdln, bg, list);
			}
		} else {
			//no pipes, just execute normally, accounting for redirects
			pid_t p = fork();
			if(p == 0) {
				child_process(tokens, commands, paths, &total_commands);
			} else {
				parent_process(p, cmdln, bg, list);
			}
		}
		total_commands += numPipes;
		//printf("cmds so far: %d\n", total_commands);

		free(input);
		free_tokens(paths);
		free_tokens(tokens);
	}

	return 0;
}

void sig_handler(int sig) {
	pid_t p = waitpid(-1, 0, WNOHANG);
	while(p > 0) {
		job *finished = find_job(p, list);
		finished->status = 0;
		if(finished != NULL) {
			remove_job(finished, list);
		}
		int fmt = 30;
		printf("\n[%d]+ %d%*s\n", finished->jid+1, finished->pid, fmt, finished->cmd_line);
		p = waitpid(-1, 0, WNOHANG);
	}
}

void child_process(tokenlist* tokens, command** commands, tokenlist *paths, int *total) {
	int fd_in = input_redirection(tokens);
	int fd_out = output_redirection(tokens);
	char* cmdPath = get_executable_path(commands[0]->tokens->items[0], paths);
	//if(cmdPath == NULL)	*total--;

	execv(cmdPath, commands[0]->tokens->items);
}

void parent_process(pid_t pid, char* cmdln, int bg, joblist* list) {
	if(bg == 1) {
		job *newJob = set_job(pid, cmdln, bg, list);
		report_job(newJob);
	} else {
		waitpid(pid, NULL, 0);
	}
}

char *get_input(void) {
	char *buffer = NULL;
	int bufsize = 0;

	char line[5];
	while (fgets(line, 5, stdin) != NULL) {
		int addby = 0;
		char *newln = strchr(line, '\n');
		if (newln != NULL)
			addby = newln - line;
		else
			addby = 5 - 1;

		buffer = (char *) realloc(buffer, bufsize + addby);
		memcpy(&buffer[bufsize], line, addby);
		bufsize += addby;

		if (newln != NULL)
			break;
	}

	buffer = (char *) realloc(buffer, bufsize + 1);
	buffer[bufsize] = 0;

	return buffer;
}

//part 5 parsing path function
tokenlist *get_path_tokens(char *path) {
	char *buff = (char *) malloc(strlen(path)+1);
	strcpy(buff, path);

	tokenlist *pathList = new_tokenlist();
	char *currPath = strtok(buff, ":");
	while(currPath != NULL) {
		add_token(pathList, currPath);
		currPath = strtok(NULL, ":");
	}

	free(buff);
	return pathList;

}

bool isPath(char *cmd) {
	if(!cmd) {
		return false;
	}
	int n = strlen(cmd);
	for(int i = 0; i < n; i++) {
		if(cmd[i] == '/') {
			return true;
		}
	}
	return false;
}

char* get_executable_path(char *cmd, tokenlist *paths) {
	char *path;
	int found;
	if(!isPath(cmd)) {
		for(int i = 0; i < paths->size; i++) {
			found = check_files(paths->items[i], cmd);
			if(found) {
				path = paths->items[i];
				break;
			}
		}
		if(!found) {
			perror("");
			return NULL;
		} else {
			char *exe = (char *const) malloc(strlen(path) + strlen(cmd) + 1);
			strcpy(exe, path);
			strcat(exe, "/");
			strcat(exe, cmd);
			return exe;
		} 
	} else {
		return cmd;
	}
}

int check_files(char* dir, char *cmd) {
	struct dirent *iterator;
	DIR *directory;
	if((directory = opendir(dir)) != NULL) {
		while((iterator = readdir(directory)) != NULL) {
			//printf("%s\n", iterator->d_name);
			if(strcmp(iterator->d_name, cmd) == 0) {
				return 1;
			}
		}
	}
	return 0;
}

int input_redirection(tokenlist *tokens) {
	char * input_file;
	int fd = -2;
	for(int i = 0; i < tokens->size; i++) {
		if(!strcmp(tokens->items[i], "<")) {
			if(i == 0 || i == tokens->size - 1) {
				fd = -1;
				printf("Invalid Index (%d)", i);
				break;
			}
			input_file = tokens->items[i+1];
			if((fd = open(input_file, O_RDONLY)) == -1) {
				perror("");
				break;
			}
			close(0);
			dup(fd);
			close(fd);
		}
	}
	return fd;
}

int output_redirection(tokenlist *tokens) {
	char * output_file;
	int fd = -2;
	for(int i = 0; i < tokens->size; i++) {
		if(!strcmp(tokens->items[i], ">")) {
			if(i == 0 || i == tokens->size - 1) {
				fd = -1;
				printf("Invalid Index (%d)", i);
				break;
			}
			output_file = tokens->items[i+1];
			if((fd = open(output_file,O_RDWR|O_CREAT,0666)) == -1) {
				perror("");
				break;
			}
			close(1);
			dup(fd);
			close(fd);
		}
	}
	return fd;
}

int link_commands(int fd_in, int fd_out, command *command, tokenlist* paths) {
	pid_t c_pid;
	if((c_pid = fork()) == 0) {
		if(fd_in != 0) {
			close(0);
			dup(fd_in);
			close(fd_in);
		}
		if(fd_out != 1) {
			close(1);
			dup(fd_out);
			close(fd_out);
		}
		char* cmdPath = get_executable_path(command->tokens->items[0], paths);
		execv(cmdPath, command->tokens->items);
	}
	return c_pid;
}

void chain_pipes(int numPipes, command** commands, tokenlist* paths, int *total) {
	int i;
	pid_t pid;
	int p_fds[2];

	int fd_in = 0;
	for(i = 0; i < numPipes-1; i++) {
		pipe(p_fds);
		link_commands(fd_in, p_fds[1], commands[i], paths);
		close(p_fds[1]);
		fd_in = p_fds[0];
	}
	if(fd_in != 0) {
		close(0);
		dup(fd_in);
	}
	char* cmdPath = get_executable_path(commands[i]->tokens->items[0], paths);
	//if(cmdPath == NULL)	*total--;

	execv(cmdPath, commands[i]->tokens->items);
}

int count_pipes(tokenlist* tokens) {
	int pipes = 0;
	for(int i = 0; i < tokens->size; i++) {
		if(!strcmp(tokens->items[i], "|")) {
			pipes++;
		}
	}
	return pipes;
}

char* get_cmdln(char* input, tokenlist* tokens, int echo) {
	//check for environmental or else overflows memory (too many characters for allocated space)
	int len = 0;
	for(int i = 0; i < tokens->size; i++) {
		len += strlen(tokens->items[i]);
	}
	char *cmdln = (char *) malloc(len + tokens->size + 1); 
	int i = echo + 1;

	strcpy(cmdln, tokens->items[echo]);
	for(; i < tokens->size; i++) {
		if(echo == 0 && i == tokens->size - 1) {
			break;
		}
		strcat(cmdln, " ");
		strcat(cmdln, tokens->items[i]);
	}

	return cmdln;
}