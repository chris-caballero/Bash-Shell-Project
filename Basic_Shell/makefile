#compiler
CC = gcc

#flags
CFLAGS = -std=c99

#target executable
TARGET = shell

#reference files (header definitions)
REF_1 = token
REF_2 = command
REF_3 = jobs

#reference file path
RPATH_1 = ./Token/
RPATH_2 = ./Command/
RPATH_3 = ./Jobs/

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(RPATH_1)$(REF_1).c $(RPATH_2)$(REF_2).c $(RPATH_3)$(REF_3).c $(TARGET).c -o $(TARGET)

clean:
	$(RM) $(TARGET)
