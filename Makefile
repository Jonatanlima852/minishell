CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: minishell

minishell: minishell.c
	$(CC) $(CFLAGS) minishell.c -o minishell

clean:
	rm -f minishell
