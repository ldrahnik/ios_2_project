#
# Projekt: River Crossing Problem
# Autor:   Luk� Drahn�k
# Datum:   24.4.2014
# 
NAME=rivercrossing
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread  # parametry p�eklada�e
SOURCES=src/*.c src/*.h

$(NAME): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@
