#
# Projekt: River Crossing Problem
# Autor:   Luk� Drahn�k
# Datum:   24.4.2014
# 
 
CC=gcc                              # p�eklada� jazyka C
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic  # parametry p�eklada�e
SOURCES=rivescrossing.c

hello: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@
