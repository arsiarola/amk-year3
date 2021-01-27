FILES = $(shell ls *.c)
list = $(shell ls)
CC := gcc
CFLAGS := --Wall -Wextra -Wfatal-errors -Wpedantic

test: $(FILES)
	gcc  -c $(FILES)

.PHONY: clean
clean:
	rm main.o random.o
