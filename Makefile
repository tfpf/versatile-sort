SHELL  = /bin/sh
CC     = g++
CFLAGS = -O2 -Wall -Wextra

Source = quicksort.cc
Binary = quicksort

.PHONY: comp

comp:
	$(CC) $(CFLAGS) -o $(Binary) $(Source)
