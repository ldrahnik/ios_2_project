#
# Projekt: River Crossing Problem
# Autor:   Lukáš Drahník
# Datum:   24.4.2014
# 
NAME=rivercrossing
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread  # parametry pøekladaèe
SOURCES=rivercrossing.c

$(NAME): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@
