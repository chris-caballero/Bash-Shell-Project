# Basic-Shell
Implements a basic shell using C

## Tar Archive:
#### Basic_Shell has everything the shell needs to be made and ran
    (1) Token has the .h and .c files for the token and tokenlist structs 
	    (a) token.h has the structs and respective function declarations
	    (b) token.c has the function definitions
	    (c) SIDE NOTE: has a lot of the given parser code since it works very well
	    (d) used to parse text into tokens and store them into lists of tokens
    (2) Command has the .h and .c files for the command struct
        (a) command.h has the command struct and function declarations
        (b) command.c has the function definitions
        (c) used to store commands and important information about them
    (3) Jobs has the .h and .c files for the job and joblist structs
        (a) job.h has the structs and respective function definitions
        (b) job.c has the function definitons
        (c) used to store a background process and information about it into a unit job 
            - then we can store the units into lists of jobs
    (4) shell.c is our main file
        (a) uses all of the previous structs to help it run more efficiently
        (b) has many helper functions within to make the main process more readable
        (c) runs a basic version of a shell!
#### README.md (this file...)
#### GIT_Log is the commit log for our repository

## Makefile:
	(1) There will not be an executable when you first use this so running "make"
	    from within the Basic_Shell folder should work to compile the processes.
	(2) I am a bit rusty with makefiles so I kind of just did it in a long string.
	(3) Once you type run 'make', you can run the shell using './shell'
	(4) To remove the executable type 'make clean'

## Known Bugs:
	(1) The number of processes shown at the end of execution may be wrong if an invalid process
	    is passed in. I was having trouble updating a pointers value from within some functions, 
	    so I couldn't get it to update if the command failed.
	(2) Doesn't always report an error when you type an invalid command (not sure why).
	(3) Background processes can report right when your command prompt is trying to load and it will 
	    sort of eat the text printed out. I think it might be that stdout wasn't flushing, but I'm not sure.
	(4) Niche scenarios may cause seg fault because I am handling the memory assuming that certain conditions are met.
