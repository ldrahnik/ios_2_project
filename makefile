#
# Projekt: River Crossing Problem
# Autor:   Lukáš Drahník
# Datum:   24.4.2014
# 
 
CC=gcc                              # pøekladaè jazyka C
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic  # parametry pøekladaèe
SOURCES=rivescrossing.c

hello: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@
